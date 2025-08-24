/******************************************************************************
 * File: PerformanceProfiler.cpp
 * Description: Implementation of performance profiling and monitoring
 * 
 * Copyright (c) 2025 Spectral Audio Systems
 ******************************************************************************/

#include "PerformanceProfiler.h"
#include <algorithm>
#include <cmath>
#include <sstream>

//==============================================================================
// Global profiler instance
//==============================================================================

static std::unique_ptr<PerformanceProfiler> globalProfiler;
static std::mutex globalProfilerMutex;

PerformanceProfiler& getGlobalProfiler()
{
    std::lock_guard<std::mutex> lock(globalProfilerMutex);
    if (!globalProfiler)
        globalProfiler = std::make_unique<PerformanceProfiler>();
    return *globalProfiler;
}

//==============================================================================
// Constructor & Destructor
//==============================================================================

PerformanceProfiler::PerformanceProfiler()
{
    // Initialize pipeline data storage
    for (int i = 0; i < pipelineData.size(); ++i)
    {
        pipelineData[i] = std::make_unique<TimingData>();
    }
    
    lastAlertCheck = std::chrono::system_clock::now();
}

PerformanceProfiler::~PerformanceProfiler() = default;

//==============================================================================
// ScopedTimer Implementation
//==============================================================================

PerformanceProfiler::ScopedTimer::ScopedTimer(PerformanceProfiler& profiler, const std::string& name)
    : profiler(profiler), timerName(name)
{
    startTime = std::chrono::high_resolution_clock::now();
}

PerformanceProfiler::ScopedTimer::~ScopedTimer()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    profiler.recordTiming(timerName, static_cast<double>(duration.count()));
}

PerformanceProfiler::ScopedTimer PerformanceProfiler::createScopedTimer(const std::string& name)
{
    return ScopedTimer(*this, name);
}

//==============================================================================
// Core Profiling Interface
//==============================================================================

void PerformanceProfiler::startTimer(const std::string& name)
{
    if (!profilingEnabled.load()) return;
    
    juce::ScopedLock lock(activeTimersLock);
    activeTimers[name] = {std::chrono::high_resolution_clock::now(), name};
}

void PerformanceProfiler::endTimer(const std::string& name)
{
    if (!profilingEnabled.load()) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    std::chrono::high_resolution_clock::time_point startTime;
    bool timerFound = false;
    
    // Get the start time under lock
    {
        juce::ScopedLock lock(activeTimersLock);
        auto it = activeTimers.find(name);
        if (it != activeTimers.end())
        {
            startTime = it->second.startTime;
            timerFound = true;
            activeTimers.erase(it);
        }
    }
    
    // Record timing without lock
    if (timerFound)
    {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        recordTiming(name, static_cast<double>(duration.count()));
    }
}

void PerformanceProfiler::recordTiming(const std::string& name, double microseconds)
{
    if (!profilingEnabled.load()) return;
    
    addTimingSample(name, microseconds);
}

//==============================================================================
// Pipeline Monitoring
//==============================================================================

void PerformanceProfiler::startPipelineStage(PipelineStage stage)
{
    if (!profilingEnabled.load()) return;
    
    std::string stageName = pipelineStageToString(stage);
    startTimer(stageName);
}

void PerformanceProfiler::endPipelineStage(PipelineStage stage)
{
    if (!profilingEnabled.load()) return;
    
    std::string stageName = pipelineStageToString(stage);
    endTimer(stageName);
}

void PerformanceProfiler::recordPipelineExecution(const PipelineExecution& execution)
{
    if (!profilingEnabled.load()) return;
    
    recordTiming("Pipeline_PaintCapture", execution.paintCapture_us);
    recordTiming("Pipeline_SpatialGridLookup", execution.spatialGridLookup_us);
    recordTiming("Pipeline_ParameterMapping", execution.parameterMapping_us);
    recordTiming("Pipeline_SampleSelection", execution.sampleSelection_us);
    recordTiming("Pipeline_AudioProcessing", execution.audioProcessing_us);
    recordTiming("Pipeline_BufferOutput", execution.bufferOutput_us);
    recordTiming("Pipeline_TotalLatency", execution.totalLatency_us);
    
    // Check for performance alerts
    if (!execution.meetsLatencyTarget(latencyTarget.load()))
    {
        addAlert(PerformanceAlert::Severity::Warning, 
                "Pipeline execution exceeded latency target: " + 
                std::to_string(execution.totalLatency_us) + "µs", "latency");
    }
}

//==============================================================================
// Performance Statistics
//==============================================================================

PerformanceProfiler::TimingStats PerformanceProfiler::getTimingStats(const std::string& name) const
{
    TimingStats stats;
    stats.name = name;
    
    juce::ScopedLock lock(timingDataLock);
    auto it = timingData.find(name);
    if (it != timingData.end() && it->second)
    {
        const auto& samples = it->second->samples;
        if (!samples.empty())
        {
            stats.updateFromSamples(samples);
            stats.targetTime_us = latencyTarget.load();
            stats.exceedsTarget = stats.averageTime_us > stats.targetTime_us;
        }
    }
    
    return stats;
}

std::vector<PerformanceProfiler::TimingStats> PerformanceProfiler::getAllTimingStats() const
{
    std::vector<TimingStats> allStats;
    
    juce::ScopedLock lock(timingDataLock);
    for (const auto& pair : timingData)
    {
        if (pair.second && !pair.second->samples.empty())
        {
            allStats.push_back(getTimingStats(pair.first));
        }
    }
    
    return allStats;
}

PerformanceProfiler::TimingStats PerformanceProfiler::getPipelineStats(PipelineStage stage) const
{
    std::string stageName = pipelineStageToString(stage);
    return getTimingStats("Pipeline_" + stageName);
}

std::vector<PerformanceProfiler::TimingStats> PerformanceProfiler::getAllPipelineStats() const
{
    std::vector<TimingStats> pipelineStats;
    
    for (int i = 0; i < 7; ++i) // 7 pipeline stages
    {
        auto stage = static_cast<PipelineStage>(i);
        auto stats = getPipelineStats(stage);
        if (stats.sampleCount > 0)
        {
            pipelineStats.push_back(stats);
        }
    }
    
    return pipelineStats;
}

void PerformanceProfiler::TimingStats::updateFromSamples(const std::vector<double>& samples)
{
    if (samples.empty()) return;
    
    sampleCount = static_cast<uint64_t>(samples.size());
    
    // Calculate basic statistics
    totalTime_us = 0.0;
    minTime_us = std::numeric_limits<double>::max();
    maxTime_us = 0.0;
    
    for (double sample : samples)
    {
        totalTime_us += sample;
        minTime_us = std::min(minTime_us, sample);
        maxTime_us = std::max(maxTime_us, sample);
        
        if (sample > targetTime_us)
            exceedCount++;
    }
    
    averageTime_us = totalTime_us / static_cast<double>(sampleCount);
    
    // Calculate advanced statistics
    std::vector<double> sortedSamples = samples;
    std::sort(sortedSamples.begin(), sortedSamples.end());
    
    // Median
    if (sortedSamples.size() % 2 == 0)
    {
        median_us = (sortedSamples[sortedSamples.size() / 2 - 1] + 
                    sortedSamples[sortedSamples.size() / 2]) / 2.0;
    }
    else
    {
        median_us = sortedSamples[sortedSamples.size() / 2];
    }
    
    // Percentiles
    size_t p95_index = static_cast<size_t>(sortedSamples.size() * 0.95);
    size_t p99_index = static_cast<size_t>(sortedSamples.size() * 0.99);
    
    percentile95_us = sortedSamples[std::min(p95_index, sortedSamples.size() - 1)];
    percentile99_us = sortedSamples[std::min(p99_index, sortedSamples.size() - 1)];
    
    // Standard deviation
    double variance = 0.0;
    for (double sample : samples)
    {
        double diff = sample - averageTime_us;
        variance += diff * diff;
    }
    variance /= static_cast<double>(sampleCount);
    standardDeviation_us = std::sqrt(variance);
}

//==============================================================================
// Real-Time Monitoring
//==============================================================================

PerformanceProfiler::PerformanceSnapshot PerformanceProfiler::getCurrentSnapshot() const
{
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    
    // Get recent performance metrics
    auto totalLatencyStats = getTimingStats("Pipeline_TotalLatency");
    if (totalLatencyStats.sampleCount > 0)
    {
        snapshot.recentAverageLatency_us = totalLatencyStats.averageTime_us;
        snapshot.recentMaxLatency_us = totalLatencyStats.maxTime_us;
    }
    
    // CPU and memory usage (simplified - would need platform-specific code for real implementation)
    snapshot.currentCpuUsage = 0.0; // TODO: Implement platform-specific CPU monitoring
    snapshot.currentMemoryUsage = 0.0; // TODO: Implement memory monitoring
    
    // Count recent dropouts (samples that exceeded latency target)
    snapshot.recentDropouts = static_cast<uint32_t>(totalLatencyStats.exceedCount);
    
    return snapshot;
}

std::vector<PerformanceProfiler::PerformanceAlert> PerformanceProfiler::getRecentAlerts() const
{
    juce::ScopedLock lock(alertsLock);
    return recentAlerts;
}

void PerformanceProfiler::clearAlerts()
{
    juce::ScopedLock lock(alertsLock);
    recentAlerts.clear();
}

//==============================================================================
// Configuration & Control
//==============================================================================

void PerformanceProfiler::reset()
{
    juce::ScopedLock lock(timingDataLock);
    timingData.clear();
    
    for (auto& pipelineDataPtr : pipelineData)
    {
        if (pipelineDataPtr)
            pipelineDataPtr->samples.clear();
    }
    
    clearAlerts();
}

void PerformanceProfiler::resetPipelineStats()
{
    juce::ScopedLock lock(timingDataLock);
    
    // Clear pipeline-specific data
    for (auto& pipelineDataPtr : pipelineData)
    {
        if (pipelineDataPtr)
            pipelineDataPtr->samples.clear();
    }
    
    // Clear pipeline timing data from main storage
    std::vector<std::string> pipelineKeys;
    for (const auto& pair : timingData)
    {
        if (pair.first.find("Pipeline_") == 0)
        {
            pipelineKeys.push_back(pair.first);
        }
    }
    
    for (const auto& key : pipelineKeys)
    {
        timingData.erase(key);
    }
}

//==============================================================================
// Export & Reporting
//==============================================================================

PerformanceProfiler::PerformanceReport PerformanceProfiler::generateReport() const
{
    PerformanceReport report;
    report.reportTime = std::chrono::system_clock::now();
    report.timingStats = getAllTimingStats();
    report.pipelineStats = getAllPipelineStats();
    report.alerts = getRecentAlerts();
    report.currentSnapshot = getCurrentSnapshot();
    
    // Calculate overall health score
    report.overallHealthScore = calculateHealthScore();
    report.meetsPerformanceRequirements = report.overallHealthScore > 0.8;
    
    // Generate recommendations
    std::ostringstream recommendations;
    if (report.overallHealthScore < 0.5)
    {
        recommendations << "CRITICAL: System performance is severely degraded. ";
    }
    else if (report.overallHealthScore < 0.8)
    {
        recommendations << "WARNING: System performance needs attention. ";
    }
    
    // Specific recommendations based on pipeline stats
    for (const auto& stats : report.pipelineStats)
    {
        if (stats.averageTime_us > stats.targetTime_us)
        {
            recommendations << "Optimize " << stats.name << " (avg: " 
                           << std::fixed << std::setprecision(1) << stats.averageTime_us 
                           << "µs). ";
        }
    }
    
    report.recommendations = recommendations.str();
    
    return report;
}

std::string PerformanceProfiler::generateTextReport() const
{
    auto report = generateReport();
    std::ostringstream oss;
    
    oss << "=== SpectralCanvas Pro Performance Report ===\n";
    oss << "Generated: " << std::chrono::duration_cast<std::chrono::seconds>(
              report.reportTime.time_since_epoch()).count() << "\n";
    oss << "Overall Health Score: " << std::fixed << std::setprecision(2) 
        << report.overallHealthScore << "/1.0\n";
    oss << "Meets Requirements: " << (report.meetsPerformanceRequirements ? "YES" : "NO") << "\n\n";
    
    // Pipeline performance
    oss << "=== Pipeline Performance ===\n";
    for (const auto& stats : report.pipelineStats)
    {
        oss << stats.name << ":\n";
        oss << "  Average: " << std::fixed << std::setprecision(1) << stats.averageTime_us << "µs\n";
        oss << "  95th Percentile: " << stats.percentile95_us << "µs\n";
        oss << "  Max: " << stats.maxTime_us << "µs\n";
        oss << "  Samples: " << stats.sampleCount << "\n";
        oss << "  Exceeds Target: " << (stats.exceedsTarget ? "YES" : "NO") << "\n\n";
    }
    
    // Current snapshot
    oss << "=== Current Status ===\n";
    oss << "Recent Average Latency: " << std::fixed << std::setprecision(1) 
        << report.currentSnapshot.recentAverageLatency_us << "µs\n";
    oss << "Recent Max Latency: " << report.currentSnapshot.recentMaxLatency_us << "µs\n";
    oss << "Recent Dropouts: " << report.currentSnapshot.recentDropouts << "\n";
    oss << "System Healthy: " << (report.currentSnapshot.isHealthy() ? "YES" : "NO") << "\n\n";
    
    // Recommendations
    if (!report.recommendations.empty())
    {
        oss << "=== Recommendations ===\n";
        oss << report.recommendations << "\n\n";
    }
    
    // Recent alerts
    if (!report.alerts.empty())
    {
        oss << "=== Recent Alerts ===\n";
        for (const auto& alert : report.alerts)
        {
            oss << "[" << (alert.severity == PerformanceAlert::Severity::Critical ? "CRITICAL" :
                          alert.severity == PerformanceAlert::Severity::Warning ? "WARNING" : "INFO")
                << "] " << alert.message << "\n";
        }
    }
    
    return oss.str();
}

bool PerformanceProfiler::exportReportToFile(const juce::File& outputFile) const
{
    auto textReport = generateTextReport();
    return outputFile.replaceWithText(textReport);
}

//==============================================================================
// Private Implementation
//==============================================================================

void PerformanceProfiler::TimingData::addSample(double microseconds)
{
    samples.push_back(microseconds);
    totalSamples.fetch_add(1);
    lastUpdate = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::TimingData::trimToMaxSize(uint32_t maxSize)
{
    if (samples.size() > maxSize)
    {
        // Keep the most recent samples
        samples.erase(samples.begin(), samples.begin() + (samples.size() - maxSize));
    }
}

std::string PerformanceProfiler::pipelineStageToString(PipelineStage stage) const
{
    switch (stage)
    {
        case PipelineStage::PaintCapture: return "PaintCapture";
        case PipelineStage::SpatialGridLookup: return "SpatialGridLookup";
        case PipelineStage::ParameterMapping: return "ParameterMapping";
        case PipelineStage::SampleSelection: return "SampleSelection";
        case PipelineStage::AudioProcessing: return "AudioProcessing";
        case PipelineStage::BufferOutput: return "BufferOutput";
        case PipelineStage::TotalLatency: return "TotalLatency";
        default: return "Unknown";
    }
}

PerformanceProfiler::TimingData* PerformanceProfiler::getOrCreateTimingData(const std::string& name)
{
    auto it = timingData.find(name);
    if (it == timingData.end())
    {
        it = timingData.emplace(name, std::make_unique<TimingData>()).first;
    }
    return it->second.get();
}

void PerformanceProfiler::addTimingSample(const std::string& name, double microseconds)
{
    bool exceedsTarget = false;
    
    // Update timing data under lock
    {
        juce::ScopedLock lock(timingDataLock);
        
        auto* data = getOrCreateTimingData(name);
        if (data)
        {
            data->addSample(microseconds);
            data->trimToMaxSize(maxSampleHistory);
        }
        
        // Check for performance alerts
        exceedsTarget = microseconds > latencyTarget.load();
    }
    
    // Add alert without lock
    if (exceedsTarget)
    {
        addAlert(PerformanceAlert::Severity::Warning,
                name + " exceeded target: " + std::to_string(microseconds) + "µs", "latency");
    }
}

void PerformanceProfiler::addAlert(PerformanceAlert::Severity severity, const std::string& message, const std::string& category)
{
    juce::ScopedLock lock(alertsLock);
    
    PerformanceAlert alert;
    alert.severity = severity;
    alert.message = message;
    alert.timestamp = std::chrono::system_clock::now();
    alert.category = category;
    
    recentAlerts.push_back(alert);
    
    // Keep only recent alerts (last 100)
    if (recentAlerts.size() > 100)
    {
        recentAlerts.erase(recentAlerts.begin());
    }
}

double PerformanceProfiler::calculateHealthScore() const
{
    // Simple health score calculation based on latency performance
    auto totalLatencyStats = getTimingStats("Pipeline_TotalLatency");
    
    if (totalLatencyStats.sampleCount == 0)
        return 1.0; // No data = assume healthy
    
    double targetLatency = latencyTarget.load();
    double averageLatency = totalLatencyStats.averageTime_us;
    
    // Score based on how well we meet the latency target
    if (averageLatency <= targetLatency)
    {
        return 1.0; // Perfect score
    }
    else
    {
        // Linear decay as latency increases beyond target
        double overage = averageLatency - targetLatency;
        double maxOverage = targetLatency; // 100% overage = 0 score
        return std::max(0.0, 1.0 - (overage / maxOverage));
    }
}