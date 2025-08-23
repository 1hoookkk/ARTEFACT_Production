# SpectralCanvas Pro Documentation

## 🎯 Quick Navigation

### For Developers
- **[Architecture Overview](architecture/)** - Core systems, DSP pipeline, RT-safety
- **[Agent Collaboration](agents/)** - AI-assisted development workflow
- **[Operations Guide](operations/)** - Build, test, deployment procedures

### For Users
- **[User Guide](UI_COMPONENTS.md)** - Interface and feature documentation
- **[Installation](ARCHIVE_POLICY.md)** - Setup and configuration

### For Contributors  
- **[Session Notes](session-notes/)** - Development history and decisions
- **[Status & Planning](../STATUS.md)** - Current project state

## 🔧 Development Workflow

### 1. Agent-Assisted Development
Use specialized agents for domain-specific tasks:
- `*agent rt-audio-guardian` - Real-time safety enforcement
- `*agent dsp-frequency-oracle` - DSP validation and frequency mapping
- `*agent juce-integration-specialist` - JUCE framework best practices
- `*agent build-stability-monitor` - Build system health

### 2. RECON → SPEC → PATCH → VERIFY Cycle
1. **RECON**: Gather evidence with reader-agent
2. **SPEC**: Define minimal change surface  
3. **PATCH**: Create surgical diffs
4. **VERIFY**: Build and test validation

### 3. Quality Gates
- **RT-Safety**: Zero locks/allocations/logging on audio thread
- **Build Stability**: Cross-platform compilation prediction
- **Mathematical Accuracy**: DSP validation with golden references
- **JUCE Compliance**: Framework integration best practices

## 📁 Documentation Structure

```
docs/
├── README.md ..................... This overview
├── architecture/ ................. Core system design
│   ├── dsp-pipeline.md ........... Audio processing architecture
│   ├── rt-safety.md .............. Real-time safety requirements
│   └── spectral-mapping.md ....... Frequency mapping mathematics
├── agents/ ....................... AI collaboration system
│   ├── contracts/ ................ Agent specifications
│   ├── knowledge/ ................ Domain-specific knowledge
│   └── workflows/ ................ Multi-agent procedures
├── operations/ ................... Development & deployment
│   ├── build-guide.md ............ Build system and commands
│   ├── testing-strategy.md ....... Testing procedures
│   └── deployment.md ............. Release and distribution
└── session-notes/ ................ Historical context
    ├── 2025-08-20.md ............. Daily development logs
    └── archive/ .................. Restored archived content
```

## 🚀 Getting Started

### For New Developers
1. Read [Architecture Overview](architecture/) to understand the system
2. Review [RT-Safety Requirements](architecture/rt-safety.md) - **Critical**
3. Set up build environment using [Build Guide](operations/build-guide.md)
4. Explore [Agent System](agents/) for AI-assisted development

### For Quick Tasks
1. Use `*help` to see available BMad commands
2. Transform into specialist agent: `*agent <name>`
3. Execute specific tasks: `*task <task-name>`
4. Generate reports: `*report` or `*doc-out`

## 🔗 External Resources

- **[CLAUDE.md](../CLAUDE.md)** - Complete development workflow
- **[STATUS.md](../STATUS.md)** - Current project status
- **[CHANGELOG.md](../CHANGELOG.md)** - Version history

---

*This documentation is maintained through agent-assisted workflows. Use `*agent` commands to access specialized knowledge and ensure accuracy.*