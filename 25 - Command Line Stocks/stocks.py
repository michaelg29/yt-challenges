import csv
import requests

class Instance:
    def __init__(self):
        self.filepath = ""
        self.apikey = ""
        self.validKey = False
        self.baseURL = "https://cloud.iexapis.com/stable"

        # instance data
        self.balance = 0.0
        self.shares = {}

    def craftURL(self, quoteSymbol):
        return f"{self.baseURL}/stock/{quoteSymbol}/quote?token={self.apikey}"

    def resetData(self):
        self.balance = 0.0
        self.shares = {}

    def insertTransaction(self, ticker, noPurchased, pricePer, writeToFile):
        # write to file if necessary
        if writeToFile:
            try:
                with open(self.filepath, "a") as f:
                    f.write(f"\n{ticker},{noPurchased},{pricePer}")
            except:
                return False

        # insert into the game data
        if ticker not in self.shares:
            self.shares[ticker] = [0, 0.0] # (noShares, totalValue)
        
        changeInBalance = round(float(noPurchased) * pricePer, 4)

        self.shares[ticker][0] += noPurchased
        self.shares[ticker][1] += changeInBalance

        self.balance -= changeInBalance

        if self.shares[ticker][0] == 0:
            # no more shares in the company
            self.shares.pop(ticker)

        return True

    def lookup(self, ticker, testKey=False):
        if ticker == "GME":
            return None

        if not(testKey or self.validKey): # De Morgan for !testKey and !self.validKey
            return None

        try:
            response = requests.get(self.craftURL(ticker))
            if response.status_code != 200:
                # status is not ok
                return None

            quote = response.json()

            return {
                "symbol": quote["symbol"],
                "name": quote["companyName"],
                "price": float(quote["latestPrice"]),
                "change": quote["change"],
                "changePercent": quote["changePercent"]
            }
        except:
            return None

    def loadAPIKey(self, path):
        self.validKey = False

        # get the key
        try:
            with open(path, "r") as f:
                self.apikey = f.read()
        except:
            return False

        # test key
        self.validKey = (self.lookup("ABCD", True) != None)

        return self.validKey

    def create(self, path, initBalance):
        self.resetData()

        if not path.endswith(".csv"):
            path += ".csv"

        try:
            with open(path, "w") as f:
                f.write(f",-1,{initBalance}")
                self.filepath = path
        except:
            return False

        self.balance = initBalance

        return True

    def load(self, path):
        self.resetData()

        if not path.endswith(".csv"):
            path += ".csv"

        try:
            with open(path, "r") as f:
                reader = csv.reader(f)

                for row in reader:
                    ticker = str(row[0])
                    noPurchased = int(row[1])
                    purchasePrice = float(row[2])

                    if len(ticker) == 0:
                        # is a deposit
                        self.balance -= round(float(noPurchased) * purchasePrice, 4)
                    else:
                        # transaction of shares
                        # insert into table
                        self.insertTransaction(ticker, noPurchased, purchasePrice, False)

                self.filepath = path
        except Exception as e:
            print(e)
            return False

        return True

    def purchase(self, ticker, noStocks):
        # get current data
        requestData = self.lookup(ticker)
        if requestData == None:
            return False

        # calculate values
        currentPrice = requestData["price"]
        totalAmount = round(noStocks * currentPrice, 4)

        if totalAmount > self.balance:
            # not enough funds
            return False

        if not self.insertTransaction(ticker, noStocks, currentPrice, True):
            # could not save
            return False

        # otherwise return summary
        return {
            "ticker": ticker,
            "noStocks": noStocks,
            "pricePer": currentPrice,
            "totalAmt": totalAmount
        }

    def sell(self, ticker, noStocks):
        # do not have any or enough shares
        if ticker not in self.shares or \
            self.shares[ticker][0] < noStocks:
            return False

        # get current data
        requestData = self.lookup(ticker)
        if requestData == None:
            return False

        # calculate values
        currentPrice = requestData["price"]
        totalAmount = round(noStocks * currentPrice, 4)

        if not self.insertTransaction(ticker, -noStocks, currentPrice, True):
            # could not save
            return False

        # otherwise return summary
        return {
            "ticker": ticker,
            "noStocks": noStocks,
            "pricePer": currentPrice,
            "totalAmt": totalAmount
        }