#!/bin/bash

# Copyright (c) 2019-2020 by Robert Bosch GmbH. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This file runs all benchmarks for Ice0ryx

COMPONENTS="utils posh"
BASE_DIR=$PWD
GBM_FILTER="*"

# check if this script is sourced by another script,
# if yes then exit properly, so the other script can use this
# scripts definitions
[[ "${#BASH_SOURCE[@]}" -gt "1" ]] && { return 0; }

if [ -z "$BM_RESULTS_DIR" ]
then
    BM_RESULTS_DIR="$(pwd)/benchmarkresults"
fi

mkdir -p "$BM_RESULTS_DIR"

echo ""
echo ""
echo ""
echo ">>>>>> Running Ice0ryx Benchmarks <<<<<<"

set -e

for COMPONENT in $COMPONENTS; do
    echo ""
    echo "######################## executing benchmarks for $COMPONENT ########################"
    cd $BASE_DIR/$COMPONENT/test

    # Runs only tests available for the given component (--benchmark_out_format={json|console|csv})
    [ -f ./"$COMPONENT"_moduletests ] && ./"$COMPONENT"_benchmarks --benchmark_filter="${GBM_FILTER}" --benchmark_out="$BM_RESULTS_DIR/"$COMPONENT"_BmResults.json" --benchmark_out_format=json

done

# do not start RouDi while the benchmarks are running

echo ">>>>>> Finished Running Iceoryx Benchmarks <<<<<<"
