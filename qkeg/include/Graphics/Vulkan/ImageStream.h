#pragma once

struct RawImageProps
{
    RawImageProps() = default;
    ~RawImageProps();

    // move
    RawImageProps(RawImageProps &&props);
    RawImageProps &operator=(RawImageProps &&props);

    // not allow copy operation
    RawImageProps(RawImageProps &props)            = delete;
    RawImageProps &operator=(RawImageProps &props) = delete;

    unsigned char *pixels{nullptr};
    int            width{0};
    int            heigh{0};
    int            channels{0};

    bool shouldFreeData{false};
};

class ImageStream
{
};