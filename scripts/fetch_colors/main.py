import requests
import re
from bs4 import BeautifulSoup, ResultSet, Tag

EXPECTED_COUNT = 256

count = 0
out = ""

html = requests.get("https://www.ditig.com/publications/256-colors-cheat-sheet").text
soup = BeautifulSoup(html, features="html5lib")
camelToSnake = re.compile(r"(?<!^)(?=[A-Z])")
rgb_extract = re.compile(r"rgb\((?:(\d+),(\d+),(\d+))\)")

rows: ResultSet[Tag] = soup.select("table tr")
for row in rows:
    cells = row.select("td")
    if len(cells) < 3:
        continue

    rgb_raw = cells[4].contents[0].text
    matches: list[tuple[str, str, str]] = rgb_extract.findall(rgb_raw)
    r, g, b = matches[0][0], matches[0][1], matches[0][2]
    out += f"RGB({r}, {g}, {b}),"
    count += 1

if count != EXPECTED_COUNT:
    print(f"Error: total amount of colors is {count} instead of {EXPECTED_COUNT}")
    exit(1)

print(out)
