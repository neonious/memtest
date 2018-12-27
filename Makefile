#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := memtest

# esp-idf or neon-esp-idf
#ESP_IDF_VERSION := esp-idf-dlmalloc
#IDF_PATH := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/../$(ESP_IDF_VERSION)

include $(IDF_PATH)/make/project.mk
