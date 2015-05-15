#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Usage: ./initialize_project.sh applicationName"
  exit
fi

sed -i '' -E 's/(<string name="app_name">)[^>]+(<\/string>)/\1'$1'\2/g' ./project_android/res/values/strings.xml
