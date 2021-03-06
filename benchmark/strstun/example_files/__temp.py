jsondata = """
{
    "12948338.sl": "MATCH {\\"num_conds\\": 2, \\"max_term_size\\": 14, \\"total_component_size\\": 30}",
    "19558979.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 5, \\"total_component_size\\": 5}",
    "2171308.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 10, \\"total_component_size\\": 10}",
    "23435880.sl": "MATCH {\\"num_conds\\": 2, \\"max_term_size\\": 14, \\"total_component_size\\": 30}",
    "25239569.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 10, \\"total_component_size\\": 10}",
    "28627624_1.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "30732554.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 14}",
    "33619752.sl": "MATCH_BELOW9 {\\"num_conds\\": 2, \\"max_term_size\\": 9, \\"total_component_size\\": 29}",
    "34801680.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 12, \\"total_component_size\\": 12}",
    "35016216.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 3, \\"total_component_size\\": 6}",
    "35744094.sl": "MATCH_BELOW9 {\\"num_conds\\": 2, \\"max_term_size\\": 3, \\"total_component_size\\": 11}",
    "37534494.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 3, \\"total_component_size\\": 6}",
    "38664547.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 5, \\"total_component_size\\": 11}",
    "38871714.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 7, \\"total_component_size\\": 7}",
    "41503046.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 20}",
    "44789427.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 20}",
    "add-a-line-break-with-a-formula.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "change-negative-numbers-to-positive.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 4, \\"total_component_size\\": 4}",
    "convert-numbers-to-text.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 2, \\"total_component_size\\": 2}",
    "convert-text-to-numbers.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 2, \\"total_component_size\\": 2}",
    "count-consecutive-monthly-orders.sl": "NONE",
    "count-line-breaks-in-cell.sl": "MATCH_BELOW9 {\\"num_conds\\": 2, \\"max_term_size\\": 9, \\"total_component_size\\": 26}",
    "count-specific-characters-in-a-cell.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 8, \\"total_component_size\\": 8}",
    "count-specific-words-in-a-cell.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 11, \\"total_component_size\\": 11}",
    "count-total-characters-in-a-cell.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 2, \\"total_component_size\\": 2}",
    "count-total-words-in-a-cell.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 10, \\"total_component_size\\": 10}",
    "create-email-address-from-name.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 7, \\"total_component_size\\": 7}",
    "create-email-address-with-name-and-domain.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "exceljet1.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "exceljet2.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "exceljet3.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 11, \\"total_component_size\\": 11}",
    "exceljet4.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "extract-text-between-parentheses.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 10, \\"total_component_size\\": 10}",
    "find-nth-occurrence-of-character.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 8, \\"total_component_size\\": 8}",
    "get-domain-from-email-address_2.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 7, \\"total_component_size\\": 7}",
    "get-domain-name-from-url.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 24}",
    "get-first-name-from-name-with-comma.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 20}",
    "get-first-name-from-name.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 7, \\"total_component_size\\": 7}",
    "get-first-word.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 7, \\"total_component_size\\": 12}",
    "get-last-line-in-cell.sl": "MATCH {\\"num_conds\\": 1, \\"max_term_size\\": 11, \\"total_component_size\\": 16}",
    "get-last-name-from-name.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "get-last-word.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "join-first-and-last-name.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 5, \\"total_component_size\\": 5}",
    "most-frequently-occurring-text.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 7, \\"total_component_size\\": 10}",
    "phone-5-short.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "phone-6-short.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "phone-7-short.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "position-of-2nd-3rd-etc-instance-of-character.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 8, \\"total_component_size\\": 8}",
    "remove-characters-from-left.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 4, \\"total_component_size\\": 4}",
    "remove-characters-from-right.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 8, \\"total_component_size\\": 8}",
    "remove-file-extension-from-filename.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 7, \\"total_component_size\\": 7}",
    "remove-leading-and-trailing-spaces-from-text.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 8, \\"total_component_size\\": 18}",
    "remove-text-by-matching.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 4, \\"total_component_size\\": 4}",
    "remove-text-by-position.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "remove-unwanted-characters.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 4, \\"total_component_size\\": 4}",
    "replace-one-character-with-another.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 4, \\"total_component_size\\": 4}",
    "split-numbers-from-units-of-measure_1.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 6, \\"total_component_size\\": 6}",
    "split-numbers-from-units-of-measure_2.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 6, \\"total_component_size\\": 6}",
    "split-text-and-numbers.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 6, \\"total_component_size\\": 6}",
    "split-text-string-at-specific-character.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 20}",
    "stackoverflow1.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "stackoverflow10.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "stackoverflow11.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 10, \\"total_component_size\\": 10}",
    "stackoverflow2.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 7, \\"total_component_size\\": 7}",
    "stackoverflow3.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "stackoverflow5.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 12, \\"total_component_size\\": 12}",
    "stackoverflow6.sl": "MATCH {\\"num_conds\\": 0, \\"max_term_size\\": 11, \\"total_component_size\\": 11}",
    "stackoverflow7.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 6, \\"total_component_size\\": 16}",
    "stackoverflow8.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "stackoverflow9.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 9, \\"total_component_size\\": 9}",
    "strip-html-from-text-or-numbers.sl": "MATCH_BELOW9 {\\"num_conds\\": 0, \\"max_term_size\\": 8, \\"total_component_size\\": 8}",
    "strip-non-numeric-characters.sl": "MATCH_BELOW9 {\\"num_conds\\": 2, \\"max_term_size\\": 9, \\"total_component_size\\": 36}",
    "strip-numeric-characters-from-cell.sl": "MATCH_BELOW9 {\\"num_conds\\": 1, \\"max_term_size\\": 9, \\"total_component_size\\": 20}"
}
"""

import json
data = json.loads(jsondata)
bench_ids = []
bench_seeds = {}
for benchid in data:
    val = data[benchid]
    benchid = benchid.replace(".sl", "")
    if val.startswith("MATCH_BELOW9"):
        bench_ids.append(benchid)
        bench_seeds[benchid] = [0, 13, 23]
final = {"bench_ids": bench_ids, "bench_seeds": bench_seeds}
print("# len(benchid)=", len(bench_ids))
print(json.dumps(final))