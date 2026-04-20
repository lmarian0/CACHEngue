import io
import os
import sys
import unittest
from contextlib import redirect_stdout
from unittest.mock import patch

TP2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
CAPA_1 = os.path.join(TP2_ROOT, "CAPA_1")

if CAPA_1 not in sys.path:
    sys.path.insert(0, CAPA_1)

import fetch_gini


class FakeResponse:
    def __init__(self, payload, ok=True, status_code=200):
        self.payload = payload
        self.ok = ok
        self.status_code = status_code

    def __bool__(self):
        return self.ok

    def __str__(self):
        return "<FakeResponse>"

    def json(self):
        return self.payload


class IntegrationAllLayersTests(unittest.TestCase):
    @patch("fetch_gini.requests.get")
    def test_full_pipeline_python_c_asm(self, mock_get):
        mock_get.return_value = FakeResponse([
            {"page": 1},
            [
                {"value": 53.2, "date": "2020", "country": {"value": "Argentina"}},
                {"value": 41.0, "date": "2019", "country": {"value": "Chile"}},
            ],
        ])

        original_cwd = os.getcwd()
        try:
            os.chdir(CAPA_1)
            buffer = io.StringIO()
            with redirect_stdout(buffer):
                fetch_gini.main()
        finally:
            os.chdir(original_cwd)

        output = buffer.getvalue()
        self.assertIn("--- INICIANDO PROCESAMIENTO EN CAPA 2 (C) ---", output)
        self.assertIn("País: Argentina | Original: 53.2 | C devuelve: 54", output)
        self.assertIn("País: Chile | Original: 41.0 | C devuelve: 42", output)


if __name__ == "__main__":
    unittest.main()
