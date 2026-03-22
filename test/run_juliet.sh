#!/bin/bash
# run_juliet.sh

PASS="$(pwd)/build/rustifyC.so"
RESULTS="test/juliet_results.txt"

echo "Test Case,Expected,Detected,Result" > $RESULTS

for testfile in test/CWE121/*.c; do
    basename=$(basename $testfile)
    
    if [[ $basename == *"_good"* ]]; then
        expected="SAFE"
    elif [[ $basename == *"_bad"* || $basename == "demo_"* ]]; then
        expected="UNSAFE"
    else
        expected="UNKNOWN"
    fi
    
    FLAGS="-Wno-everything -O0 -Xclang -disable-O0-optnone -fno-discard-value-names"
    
    # Check compile time (Static rejections)
    clang $FLAGS -g -fpass-plugin=$PASS $testfile -o /tmp/test_juliet > /tmp/out.log 2>&1
    comp_status=$?

    if grep -q "rustifyC:" /tmp/out.log || grep -q "error\[" /tmp/out.log || [ $comp_status -ne 0 ]; then
        detected="REJECT"
    else
        # Check run time (Dynamic panics)
        /tmp/test_juliet 15 > /tmp/run.log 2>&1
        run_status=$?
        if grep -q "panicked at" /tmp/run.log || grep -q "Temporal Safety Violation" /tmp/run.log || [ $run_status -ne 0 ]; then
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
    elif [[ $expected == "SAFE" && $detected == "REJECT" ]]; then
        result="FP"  # False Positive - WRONGLY FLAGGED SAFE CODE
    else
        result="MAYBE"
    fi
    
    # Format padding
    padded_name=$(printf "%-30s" "$basename")
    padded_expected=$(printf "%-8s" "$expected")
    padded_detected=$(printf "%-8s" "$detected")
    
    echo "$padded_name $padded_expected $padded_detected $result"
    echo "$basename,$expected,$detected,$result" >> $RESULTS
done

# Summary
TP=$(grep ",TP$" $RESULTS | wc -l | tr -d ' ')
TN=$(grep ",TN$" $RESULTS | wc -l | tr -d ' ')
FN=$(grep ",FN$" $RESULTS | wc -l | tr -d ' ')
FP=$(grep ",FP$" $RESULTS | wc -l | tr -d ' ')

TOTAL=$(($TP + $TN + $FP + $FN))
if [ $TOTAL -eq 0 ]; then TOTAL=1; fi # prevent div-by-zero

ACCURACY=$(echo "scale=2; 100 * ($TP + $TN) / $TOTAL" | bc)
PRECISION=$(echo "scale=2; if(($TP+$FP)>0) 100 * $TP / ($TP + $FP) else 0" | bc)
RECALL=$(echo "scale=2; if(($TP+$FN)>0) 100 * $TP / ($TP + $FN) else 0" | bc)

echo ""
echo "=== NIST JULIET VALIDATION SUMMARY ==="
echo "Total Tests:     $TOTAL"
echo "True Positives:  $TP (Correctly blocked unsafe code)"
echo "True Negatives:  $TN (Correctly allowed safe code)"
echo "False Negatives: $FN (Missed vulnerabilities)"
echo "False Positives: $FP (Wrongly blocked safe code)"
echo "--------------------------------------"
echo "Accuracy:        $ACCURACY%"
echo "Precision:       $PRECISION%"
echo "Recall:          $RECALL%"
echo "======================================"

rm -f /tmp/test_juliet /tmp/out.log /tmp/run.log
