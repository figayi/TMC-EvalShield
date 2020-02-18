#!/usr/bin/env python3
'''
Created: 13.02.2020

@author: LK
'''

import argparse
import os
import logging

COMMAND_GIT = "git"
COMMAND_MAKE = "make"
COMMAND_CHECK_CLEAN = f"{COMMAND_GIT} diff --quiet --exit-code"
COMMAND_CHECK_TAG = f"{COMMAND_GIT} describe --exact-match --tags HEAD >/dev/null 2>&1"
COMMAND_TAG = f"{COMMAND_GIT} tag %(tag)s"
COMMAND_PUSH_COMMITS = f"{COMMAND_GIT} push --recurse-submodules=on-demand"
COMMAND_PUSH_TAGS = f"{COMMAND_GIT} "

EXIT_CHECK_CLEAN = 1
EXIT_CHECK_TAG = 2

parser = argparse.ArgumentParser(description='TMC deploy script')
parser.add_argument('-b', '--build', dest='build', action='store_true',
                    help='Invoke specified build command')
parser.add_argument('-u', '--upload', dest='upload', action='append', nargs='+', type=argparse.FileType('r'),
                    help='Mark binary for upload')
parser.add_argument('-t', '--token', dest='token', action='store', nargs=1, type=str,
                    help='GitHub API v3 token')
parser.add_argument('--no-tag', dest='tag', action='store_false',
                    help='Disable tagging')
parser.add_argument('--no-version-increment', dest='version', action='store_false',
                    help='Disable version incrementing')
parser.add_argument('--no-check-clean', dest='clean', action='store_false',
                    help='Disable clean repository check')
parser.add_argument('--force-tag', dest='force_tag', action='store_true',
                    help='Force tagging even if HEAD is already tagged')
parser.add_argument('-v', '--verbosity', dest='verbosity', action='store', nargs=1, type=int, choices=[logging.DEBUG, logging.INFO, logging.WARNING, logging.ERROR, logging.CRITICAL], default=[logging.INFO],
                    help=f'Verbosity level (default: %(default)s, {logging.DEBUG}: DEBUG, {logging.INFO}: INFO, {logging.WARNING}: WARNING, {logging.ERROR}: ERROR, {logging.CRITICAL}: CRITICAL)')

args = parser.parse_args()
verbosity = args.verbosity[0]

logger = logging.getLogger(__name__)
logger.setLevel(verbosity)

consoleHandler = logging.StreamHandler()
consoleHandler.setLevel(verbosity)
formatter = logging.Formatter("[%(asctime)s] [%(name)s] [%(levelname)s] %(message)s")
consoleHandler.setFormatter(formatter)
logger.addHandler(consoleHandler)

logger.debug(f"raw_args = {{ args.build: {args.build}, args.upload: {args.upload}, args.token: {args.token}, args.verbosity: {args.verbosity} }}")

version_file = open("version.txt", 'r')
version = version_file.read().split('=')[1]
version_file.close()

logger.info(f"Detected version: {version}")

if(args.build):
    logger.info("Building.")
    logger.debug(COMMAND_MAKE)
    os.system(COMMAND_MAKE)
    logger.info("Build completed.")

if(args.clean):
    logger.info("Checking for clean repository.")
    logger.debug(COMMAND_CHECK_CLEAN)
    if(os.system(COMMAND_CHECK_CLEAN)):
        logger.error("Repository not clean. Commit or stash all changes.")
        exit(EXIT_CHECK_CLEAN)
    logger.info("Repository clean.")

if(args.tag):
    logger.info("Checking for tag.")
    logger.debug(COMMAND_CHECK_TAG)
    if(os.system(COMMAND_CHECK_TAG)):
        if(args.force_tag):
            logger.error("HEAD is already tagged. Force tag enabled, tagging anyways.")
        else:
            logger.error("HEAD is already tagged. Add flag '--force-tag' to tag anyways.")
            exit(EXIT_CHECK_TAG)
    else:
        logger.info("HEAD is not tagged.")
    logger.info("Tagging.")
    logger.debug(COMMAND_TAG % { "tag": version })
    if(os.system(COMMAND_TAG % { "tag": version })):
        logger.error("Could not create tag.")
        exit(EXIT_TAG)
    logger.info("Tagged.")
