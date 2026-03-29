#!/bin/bash
# demo_all.sh - Master Demonstration Script for RustifyC

# Ensure scripts are executable
chmod +x demo.sh demo_phase3.sh demo_phase4.sh demo_phase5.sh demo_phase6.sh

echo "========================================================================="
echo "               RUSTIFY-C COMPLETE SUITE DEMONSTRATION                    "
echo "========================================================================="
echo "This script will sequentially run the demonstrations for all phases,"
echo "showcasing the full extent of RustifyC's spatial and temporal safety."
echo "========================================================================="
echo ""
read -p "Press [Enter] to begin Phase 1 & 2 (Basic Checks and Diagnostics)..."

# Run demo.sh but strip the call to demo_phase6.sh at the very end
head -n 106 demo.sh > /tmp/demo_stripped.sh
chmod +x /tmp/demo_stripped.sh
/tmp/demo_stripped.sh

echo ""
read -p "Press [Enter] to begin Phase 3 (SCEV Optimization)..."
./demo_phase3.sh

echo ""
read -p "Press [Enter] to begin Phase 4 (Uninitialized Memory Detection)..."
./demo_phase4.sh

echo ""
read -p "Press [Enter] to begin Phase 5 (Integer Overflow Detection)..."
./demo_phase5.sh

echo ""
read -p "Press [Enter] to begin Phase 6 (Stack Escape Detection)..."
./demo_phase6.sh

echo ""
echo "========================================================================="
echo "                  ALL PHASES COMPLETED SUCCESSFULLY                      "
echo "========================================================================="
