#!/bin/bash

tests_folder="tests"

for file in "$tests_folder"/tests*.rb
do
    echo 
    echo "Running [$file].."
    ruby "$file"
done

