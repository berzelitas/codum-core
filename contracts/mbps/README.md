# Milestone Based Payment Standard

## How to build with Docker

1. Build container: `docker build -t mbps-builder .`
2. Run container: `docker run --name mbps-build mbps-builder`
3. Copy built contract: `docker cp mbps-build:/build .`
