#!/usr/bin/env bash
# based on: http://stackoverflow.com/questions/26881441/can-you-get-the-number-of-lines-of-code-from-a-github-repository
git clone --depth 1 "$1" temp-linecount-repo
printf "('temp-linecount-repo' will be deleted automatically)\n\n\n"
cloc temp-linecount-repo
rm -rf temp-linecount-repo
