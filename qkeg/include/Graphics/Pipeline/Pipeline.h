#pragma once

class GPUDevice;

class Pipeline
{
    virtual void init(GPUDevice &device)    = 0;
    virtual void cleanUp(GPUDevice &device) = 0;
    virtual void beginDraw(int frameIndex)  = 0;
};