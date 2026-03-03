#!/bin/bash
# run_juliet.sh

PASS="$(pwd)/build/rustifyC.so"
RESULTS="test/juliet_results.txt"

# In a real Juliet suite we have 200+ CWE121 files.
# For our demo MVP, we will run our mock CWE121 directory files to simulate it.

echo "Test Case,Expected,Detected,Result" > $RESULTS

for testfile in test/CWE121/*.c; do
    if [[ $testfile == *"bad"* || $testfile == *"static"* || $testfile == *"dynamic"* ]]; then
        expected="UNSAFE"
    else
        expected="SAFE"
    fi
    
    # Catch both Compile-Time (Static) Rejections and Run-Time (Dynamic) Failures
    # We grep for our custom error format or standard crash codes
    
    # Check compile time:
    clang -Wno-everything -fpass-plugin=$PASS $testfile -o /tmp/test_juliet > /tmp/out.log 2>&1
    comp_status=$?

    if grep -q "rustifyC" /tmp/out.log || [ $comp_status -ne 0 ]; then
        detected="REJECT"
    else
        # Check run time
        /tmp/test_juliet 15 > /tmp/run.log 2>&1
        run_status=$?
        if grep -q "panicked at" /tmp/run.log || [ $run_status -ne 0 ]; then
             detected="REJECT"
        else
             detected="PASS"
        fi
    fi
    
    # Compare
    if [[ $expected == "UNSAFE" && $detected == "REJECT" ]]; then
        result="TP"  # True Positive
    elif [[ $expected == "SAFE" && $detected == "PASS" ]]; then
        result="TN"  # True Negative
    elif [[ $expected == "UNSAFE" && $detected == "PASS" ]]; then
        result="FN"  # False Negative - MISSED VULNERABILITY
    else
        result="FP"  # False Positive - WRONGLY FLAGGED SAFE CODE
    fi
    
    echo "$(basename $testfile),$expected,$detected,$result" >> $RESULTS
done

# Summary
echo "=== RESULTS ==="
echo "True Positives:  $(grep ",TP$" $RESULTS | wc -l | tr -d ' ')"
echo "True Negatives:  $(grep ",TN$" $RESULTS | wc -l | tr -d ' ')"
echo "False Negatives: $(grep ",FN$" $RESULTS | wc -l | tr -d ' ')"
echo "False Positives: $(grep ",FP$" $RESULTS | wc -l | tr -d ' ')"
rm -f /tmp/test_juliet /tmp/out.log /tmp/run.log
