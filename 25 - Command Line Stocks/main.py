import sys
from stocks import Instance

def usd(val):
    return  f"{val:,.2f}"

def main():
    """
        NOTE: NO COMMAND LINE INPUT VALIDATION
    """

    print("Hello, Stock Market!")
    print("All data provided by IEX Cloud")

    inst = Instance()

    noArgs = len(sys.argv)
    if noArgs == 1:
        # need path to API key file
        print("Run format:")
        print("py main.py {apikeyfilepath} ?{instancefilepath}")
        exit(-1)

    if not inst.loadAPIKey(sys.argv[1]):
        print(
            "Please input a valid file with the API key or check your internet connection")
        exit(-1)

    if noArgs == 2:
        # start menu
        print("Welcome to Command Line Stocks")
        print("All data provided by IEX Cloud")
        while True:
            print("=" * 25)
            print("Select one option below")
            print("1 - Create New Instance")
            print("2 - Load Existing Instance")
            option = input("Selection: ")

            try:
                option = int(option)
            except:
                print("Please input an integer")
                continue

            if option < 1 or option > 2:
                print("Please input a 1 or 2")
                continue
            elif option == 1:
                # create instance
                while True:
                    # get initial balance
                    initBalance = input("Input initial balance: $")
                    try:
                        initBalance = float(initBalance)
                        break
                    except:
                        print("Please input a valid number")
                        continue

                path = input("Input desired filepath (relative or absolute): ")
                if inst.create(path, initBalance):
                    break
                else:
                    print("Please input a valid path")
            elif option == 2:
                # load filepath
                path = input("Input filepath (relative or absolute): ")
                if inst.load(path):
                    break
                else:
                    print("Please input a valid and formatted file")
    else:
        # automatically start instance
        if not inst.load(sys.argv[2]):
            print("Please input a valid and formatted file")
            exit(-1)

    """
        main loop
    """
    while True:
        ticker = input(f"Input ticker ('_' to list shares, 'q' to quit) (${usd(inst.balance)})> ")

        if ticker == 'q':
            break
        elif ticker == '_':
            # list current shares
            for t in inst.shares:
                noOwned = inst.shares[t][0]
                valOwned = inst.shares[t][1]

                curData = inst.lookup(t)

                curVal = noOwned * curData["price"]
                roi = curVal - valOwned
                gain = roi >= 0.0

                print(f"{t}: {noOwned} shares owned, currently valued at ${usd(curVal)}, for a {'gain' if gain else 'loss'} of ${usd(roi)}")
        else:
            # lookup data
            data = inst.lookup(ticker)

            if data == None:
                print("Invalid ticker")
                continue

            # print info
            print(f"""
                {data['name']} ({data['symbol']}):\n
                Currently valued at {data['price']}, {data['change']:+} ({round(data['changePercent'] * 100.0, 4):+}%)
            """)

            # print user info associated with company
            noOwned = 0
            if ticker in inst.shares:
                noOwned = inst.shares[ticker][0]
                valOwned = inst.shares[ticker][1]

                curVal = noOwned * data["price"]
                roi = curVal - valOwned
                gain = roi >= 0.0

                print(f"You own {noOwned} shares of this company, currently valued at ${usd(curVal)}, for a {'gain' if gain else 'loss'} of ${usd(roi)}")
            else:
                print("You do not own any shares of this company.")

            # get action
            while True:
                action = input("Input action (buy, sell, continue): ")
                if action == "buy" or action == "b":
                    try:
                        noStocks = int(input("Input number of stocks to buy: "))
                        if noStocks <= 0:
                            print("Please input a positive integer greater than 0")
                            continue

                        ret = inst.purchase(ticker, noStocks)
                        if not ret:
                            print("Transaction not completed or saved")
                        else:
                            print(f"""
                                Purchase of {ret['noStocks']} from {ret['ticker']} completed (-${usd(ret['totalAmt'])}).
                            """)
                            break

                    except:
                        print("Please input a valid integer")
                elif action == "sell" or action == "s":
                    if noOwned <= 0:
                        print("You cannot sell any stocks")

                    try:
                        noStocks = int(input("Input number of stocks to sell: "))
                        if noStocks <= 0:
                            print("Please input a positive integer greater than 0")
                            continue

                        ret = inst.sell(ticker, noStocks)
                        if not ret:
                            print("Transaction not completed or saved")
                        else:
                            print(f"""
                                Sale of {ret['noStocks']} from {ret['ticker']} completed (+${usd(ret['totalAmt'])}).
                            """)
                            break
                    except:
                        print("Please input a valid integer")
                elif action == "continue" or action == "c":
                    break
                else:
                    print("Invalid input")

    print("Thank you for making boatloads of money with Command Line Stocks")
    print("All data provided by IEX Cloud")

if __name__ == "__main__":
    main()