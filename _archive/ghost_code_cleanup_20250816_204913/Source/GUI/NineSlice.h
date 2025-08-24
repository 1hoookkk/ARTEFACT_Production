#pragma once
#include <JuceHeader.h>

inline void drawNineSlice(juce::Graphics& g, const juce::Image& img,
                          juce::Rectangle<int> r, juce::BorderSize<int> in)
{
    if (img.isNull() || r.isEmpty()) return;

    const int w = img.getWidth(), h = img.getHeight();
    const int L = in.getLeft(), R = in.getRight(), T = in.getTop(), B = in.getBottom();

    auto clip = [&](int x,int y,int ww,int hh){ return img.getClippedImage({x,y,ww,hh}); };
    auto fit  = [&](const juce::Image& I, juce::Rectangle<int> rr){
        g.drawImageWithin(I, rr.getX(), rr.getY(), rr.getWidth(), rr.getHeight(),
                          juce::RectanglePlacement::stretchToFit, false);
    };

    // corners
    fit(clip(0,0,L,T),                         {r.getX(), r.getY(), L, T});
    fit(clip(w-R,0,R,T),                       {r.getRight()-R, r.getY(), R, T});
    fit(clip(0,h-B,L,B),                       {r.getX(), r.getBottom()-B, L, B});
    fit(clip(w-R,h-B,R,B),                     {r.getRight()-R, r.getBottom()-B, R, B});
    // edges
    fit(clip(L,0,w-L-R,T),                     {r.getX()+L, r.getY(), r.getWidth()-L-R, T});
    fit(clip(L,h-B,w-L-R,B),                   {r.getX()+L, r.getBottom()-B, r.getWidth()-L-R, B});
    fit(clip(0,T,L,h-T-B),                     {r.getX(), r.getY()+T, L, r.getHeight()-T-B});
    fit(clip(w-R,T,R,h-T-B),                   {r.getRight()-R, r.getY()+T, R, r.getHeight()-T-B});
    // center
    fit(clip(L,T,w-L-R,h-T-B),                 {r.getX()+L, r.getY()+T, r.getWidth()-L-R, r.getHeight()-T-B});
}