#!/bin/bash

set -e

echo "=== Remove old folders ==="
./scripts/remove.sh

echo "=== Build and run ==="
./scripts/build-and-run.sh
