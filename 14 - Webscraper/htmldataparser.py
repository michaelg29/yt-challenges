from bs4 import BeautifulSoup
import requests
import json
import enum
import re

class TokenType(enum.Enum):
    Tag = 0
    Class_ = 1
    Id = 2

def processSelectStr(str, processAttrs = False):
    """
        take in:
            tag[selector1, ...]{attr1,...}.class1.class2#id
    """

    ret = {
        "tag": "",
        "class": [],
        "id": "",
        "obtainMultiple": False
    }

    if str[0] == "*":
        ret["obtainMultiple"] = True
        str = str[1:]

    currentSection = ""
    currentType = TokenType.Tag
    for c in str:
        if c in ".#":
            # end of section - set in variable
            if currentType == TokenType.Tag:
                ret["tag"] = currentSection
            elif currentType == TokenType.Class_:
                ret["class"].append(currentSection)
            else:
                ret["id"] = currentSection

            # reset type
            if c == '.':
                currentType = TokenType.Class_
            else:
                currentType = TokenType.Id

            currentSection = ""
        else:
            currentSection += c

    # last section
    if currentType == TokenType.Tag:
        ret["tag"] = currentSection
    elif currentType == TokenType.Class_:
        ret["class"].append(currentSection)
    else:
        ret["id"] = currentSection

    # get target attributes
    if processAttrs:
        attrsSearch = re.search(RegexSelectors.braceRegex, ret["tag"])
        if attrsSearch:
            attrs = ret["tag"][attrsSearch.start(0) + 1:attrsSearch.end(0) - 1]
            ret["tag"] = ret["tag"][0:attrsSearch.start(0)] + ret["tag"][attrsSearch.end(0):]

            attrs = re.sub(r"[ ]", '', attrs)
            ret["attrs"] = re.split(r"[,]", attrs)
            ret["selectStr"] = str[0:attrsSearch.start(0)] + str[attrsSearch.end(0):]
        else:
            ret["attrs"] = False
            ret["selectStr"] = str
    else:
        ret["selectStr"] = str

    return ret

def cleanText(text):
    text = re.sub(RegexSelectors.whitespaceRegex, ' ', text) # replace all new lines, etc with spaces
    text = re.sub(RegexSelectors.multipleSpaceRegex, ' ', text) # replace >= 2 spaces with 1
    text = re.sub(RegexSelectors.stripRegex, '', text) # strip text
    return text

def processElementData(element, attrs):
    """
        return format
        {
            "text": text,
            "attr1" attr1,
            ...
        }
    """

    ret = {
        "text": cleanText(element.text)
    }

    if attrs:
        for attr in attrs:
            ret[attr] = element.get(attr)

    return ret

"""
    Stores strings for general use in program
"""
class RegexSelectors:
    bracketRegex = r"\[.*?]"
    braceRegex = r"{.*?}"

    # remove all whitespace characters (excluding space)
    whitespaceRegex = r"[\t\n\r\f\v]"

    # remove instances of >= 2 spaces
    multipleSpaceRegex = r"[ ]{2,}"

    # left/right strip strings
    stripRegex = r"^[ ]+|[ ]+$"

class HtmlDataParser:
    def __init__(self, urlFormat, dataFormat):
        self.urlFormat = {}
        self.dataFormat = {}

        self.processUrlFormat(urlFormat)
        self.processDataFormat(dataFormat)

    def processUrlFormat(self, urlFormat):
        if type(urlFormat) is str:
            # is a filepath, so read file
            self.urlFormat = json.load(open(urlFormat))
        else:
            # is precompiled dictionary
            self.urlFormat = urlFormat

        # now is in json format

        for action in self.urlFormat["actions"]:
            """
                for each action, fill in nonexistent values for params/query
            """
            if "query" in self.urlFormat["actions"][action]:
                for query in self.urlFormat["actions"][action]["query"]:
                    if "vals" not in self.urlFormat["actions"][action]["query"][query]:
                        self.urlFormat["actions"][action]["query"][query]["vals"] = False
                    if "default" not in self.urlFormat["actions"][action]["query"][query]:
                        self.urlFormat["actions"][action]["query"][query]["default"] = False
            else:
                self.urlFormat["actions"][action]["query"] = False

            if "params" not in self.urlFormat["actions"][action]:
                self.urlFormat["actions"][action]["params"] = False

    def processDataFormat(self, dataFormat):
        if type(dataFormat) is str:
            # is a filepath, so read file
            self.dataFormat = json.load(open(dataFormat))
        else:
            # is precompiled dictionary
            self.dataFormat = dataFormat

        # now is in json format

        for action in self.dataFormat:
            """
                process each element in separate select str
                create top down element tree
            """

            for key in self.dataFormat[action]:
                self.dataFormat[action][key] = re.sub(r"( > )|( >)|( >)", '>', self.dataFormat[action][key])

                elements = []
                selectStrs = re.split("[>]", self.dataFormat[action][key])
                for i in range(len(selectStrs)):
                    elements.insert(0, processSelectStr(selectStrs[i], i == len(selectStrs) - 1))

                """
                    ul[aria-label='Search results']
                    *li
                    a{href}.package-snippet
                """

                # construct top down list
                previousElement = False
                for element in elements:
                    if previousElement:
                        element["child"] = previousElement

                    previousElement = element

                self.dataFormat[action][key] = previousElement

    def constructUrl(self, action, params = False, query = False):
        if action not in self.urlFormat["actions"]:
            return False

        """
            construct url
            1) get domain/route
            2) fill in parameters
            3) add query vals
        """

        # domain
        url = self.urlFormat["domain"]

        # parameters
        route = self.urlFormat["actions"][action]["route"]
        if self.urlFormat["actions"][action]["params"]:
            for param in self.urlFormat["actions"][action]["params"]:
                paramString = "{" + param + "}"
                route = re.sub(paramString, params[param], route)

        url += route

        # query
        if self.urlFormat["actions"][action]["query"]:
            queryString = ""
            for queryParam in self.urlFormat["actions"][action]["query"]:
                valid = False
                title = self.urlFormat["actions"][action]["query"][queryParam]["title"]
                if query and queryParam in query:
                    if self.urlFormat["actions"][action]["query"][queryParam]["vals"]:
                        if query[queryParam] in self.urlFormat["actions"][action]["query"][queryParam]["vals"]:
                            valid = True
                    else:
                        # no specified value
                        valid = True
                else:
                    if not self.urlFormat["actions"][action]["query"][queryParam]["default"]:
                        continue

                if valid:
                    queryString += title + "=" + str(query[queryParam])
                else:
                    # use default value
                    queryString += title + "=" + str(self.urlFormat["actions"][action]["query"][queryParam]["default"])

                queryString += '&'

            if queryString != "":
                url += "?" + queryString[:len(queryString) - 1] # remove last ampersand

        return url

    def processData(self, action, data):
        soup = BeautifulSoup(data, "html.parser")

        """
            return format
            {
                "key1": {
                    "text": text,
                    "attr1": attr1
                    ...
                }
                "key2_obtainMultiple": [
                    {
                        "text": text
                        "attr": attr
                    }
                ]
            }
        """

        ret = {}

        for key in self.dataFormat[action]:
            """
                for each key
                - traverse down the tree (using children)
                - select elements using selectStr
                - determine if attributes
            """

            select = self.dataFormat[action][key]
            elements = [ soup ]
            attrs = False
            while True:
                newElements = []
                for el in elements:
                    if el:
                        res = el.select(select["selectStr"])
                        if select["obtainMultiple"]:
                            # get all instances
                            newElements.extend(res)
                        else:
                            # get first instance
                            if res:
                                newElements.append(res[0])

                """
                    |soup|
                    |p|
                    |span1,span2,...|
                    |span1 > a, span1 > a2, span2 > a, span2 > a2, ...|
                """

                # use the elements in the next iteration
                elements = newElements

                # determine if need attributes
                if "attrs" in select:
                    attrs = select["attrs"]

                # if child exists, go down tree, else break
                if "child" in select:
                    select = select["child"]
                else:
                    # end of tree
                    break

            # take data from elements
            if len(elements) == 1:
                # only one element
                ret[key] = processElementData(elements[0], attrs)
            elif len(elements) > 1:
                # return list
                ret[key] = []
                for el in elements:
                    ret[key].append(processElementData(el, attrs))
            else:
                return False

        return ret

    def request(self, action, params = False, query = False):
        url = self.constructUrl(action, params, query)
        print(url)
        if url:
            data = requests.get(url)
            return self.processData(action, data.content)
        else:
            return False