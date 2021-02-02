# Command Line Stocks

## Setup

### Required PIP Packages
- [requests](https://pypi.org/project/requests/)
	- `python -m pip install requests`

### Downloading Files
- apikey.txt
	- You must create an account with [IEX Cloud](https://www.iexcloud.io/).
	- Get your API key from the dashboard and paste it into this file.

## Running
`py main.py apiPath.txt transactions.csv`

Or, to load up the main menu:
`py main.py apiPath.txt`

*Replace apiPath.txt with the path to the file containing your API Key.*
*Replace transactions.csv with the path to the file containing your transaction data.*
