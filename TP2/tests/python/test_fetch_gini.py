import os
import sys
import unittest
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


class FetchGiniUnitTests(unittest.TestCase):
    @patch("fetch_gini.requests.get")
    def test_apiRest_returns_json_payload(self, mock_get):
        payload = [{"page": 1}, []]
        mock_get.return_value = FakeResponse(payload)

        result = fetch_gini.apiRest()

        self.assertEqual(result, payload)
        mock_get.assert_called_once()

    @patch.object(fetch_gini, "apiRest")
    def test_get_gini_data_filters_and_deduplicates(self, mock_api_rest):
        mock_api_rest.return_value = [
            {"pages": 1},
            [
                {"value": 34.2, "date": "2020", "country": {"value": "Argentina"}},
                {"value": 35.1, "date": "2019", "country": {"value": "Argentina"}},
                {"value": None, "date": "2018", "country": {"value": "Chile"}},
                {"value": 25.0, "date": "2018", "country": {"value": "Chile"}},
            ],
        ]

        result = fetch_gini.get_gini_data()

        self.assertEqual(result, [("Argentina", "2020", 34.2), ("Chile", "2018", 25.0)])


if __name__ == "__main__":
    unittest.main()
