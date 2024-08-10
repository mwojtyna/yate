# yate

Yet Another Terminal Emulator

## Build

`cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . -j`

## Configuration

- Set log level with `SPDLOG_LEVEL` environment variable

## Todo

- [x] OpenGL setup
- [ ] Font rendering
- [ ] PTY initialization
- [ ] Terminal codes parsing
- [ ] Window resizing
