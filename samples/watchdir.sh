#!/bin/bash

if [[ $# -lt 1 ]]; then
  echo "Usage $0 <file|dir>[ <file|dir>]*"
  exit 1
fi

while file=$(./kqwait $@); do
  case "$file" in
    +*.pm)
      echo "A new perl module appeared: ${file##+ }"
      ;;
    -*.pm)
      echo "A perl module disappeared: ${file##- }"
      ;;
    *.pm)
      echo "A perl module was modified: $file"
      ;;
    *)
      echo "Ignoring $file"
      ;;
  esac
done
