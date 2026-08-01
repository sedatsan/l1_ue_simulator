# Stage 1: Build Environment
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j$(nproc)

# Stage 2: Runtime Environment
FROM ubuntu:22.04

# Set working directory
WORKDIR /app

# Copy the compiled binary from the builder stage
COPY --from=builder /app/build/l1_ue_simulator /usr/local/bin/l1_ue_simulator

# Ensure IPC socket directory exists
RUN mkdir -p /tmp

# Run the simulator
ENTRYPOINT ["/usr/local/bin/l1_ue_simulator"]
