import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "host"))
import h7_cli as m


class TestRedact(unittest.TestCase):
    def test_short(self):
        self.assertEqual(m.redact("abc"), "***")

    def test_keeps_prefix_suffix(self):
        self.assertEqual(m.redact("super8secrets")[0], "s")
        self.assertEqual(m.redact("super8secrets")[-1], "s")
        self.assertNotIn("p", m.redact("super8secrets"))  # 'p' is interior a bit wobbly

    def test_redact_hides_middle(self):
        v = m.redact("123456")
        self.assertNotEqual(v, "123456")


if __name__ == "__main__":
    unittest.main()
