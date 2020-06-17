from htmldataparser import HtmlDataParser
import json

if __name__ == "__main__":
    print("Hello, world!")

    formats = json.load(open(r"pipformat.json"))
    data = HtmlDataParser(formats["url"], formats["data"])

    while True:
        search = input("Search term: ")
        if search == "stop":
            break

        res = data.request("project", params={
            "name": search
        })
        print(res)