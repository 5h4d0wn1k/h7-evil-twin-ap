#!/usr/bin/env python3
"""H7 - Evil Twin AP host helper: offline redaction + audit of credential logs
produced during AUTHORIZED own-network captive-portal lab tests.
Educational/authorized own-lab use only (see README "IMPORTANT").
"""
import argparse
import os
import re
import sys

MOD = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, MOD)
from hw_common import DEMO_TAG, read_target

CRED_RE = re.compile(
    r"SSID:\s*(?P<ssid>\S+)\s*\n.*Password:\s*(?P<pass>`\S+)",
    re.S | re.I)


def redact(value):
    if len(value) <= 4:
        return "*" * len(value)
    return value[:1] + "*" * (len(value) - 2) + value[-1:]


def analyze(text):
    creds = []
    for m in CRED_RE.finditer(text):
        creds.append({"ssid": m.group("ssid"), "password": m.group("pass")})
    return creds


def run_demo():
    print("=== H7 own-network captive-portal audit (offline, redacted) ===")
    sample = ("SSID: lab-own-wifi\nPassword: h0n3y\nClient: 00:11:22:33:44:55\n"
              "SSID: lab-own-wifi\nPassword: super8secrets\nClient: 00:11:22:33:44:55\n")
    for c in analyze(read_target("fixtures/creds.log", sample)):
        print("  SSID=%s  password=%s" % (c["ssid"], redact(c["password"])))
    print(DEMO_TAG)
    return 0


def main(argv=None):
    p = argparse.ArgumentParser(
        description="H7 Evil Twin AP - offline credential-log audit (redacted)")
    p.add_argument("--demo", action="store_true", help="offline demo (exit 0)")
    p.add_argument("--file", help="credential log path")
    p.add_argument("--reveal", action="store_true",
                   help="show values (authorized lab only)")
    args = p.parse_args(argv)
    text = read_target("fixtures/creds.log")
    if args.file:
        text = open(args.file).read()
    if args.demo or not args.file:
        return run_demo()
    for c in analyze(text):
        pw = c["password"] if args.reveal else redact(c["password"])
        print("SSID=%s password=%s" % (c["ssid"], pw))
    return 0


if __name__ == "__main__":
    sys.exit(main())
