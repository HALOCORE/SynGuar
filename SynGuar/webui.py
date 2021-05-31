from flask import Flask, request
from flask_restful import Resource, Api
from flask_cors import CORS
import sys
import os

app = Flask(__name__, static_url_path='/ui', static_folder='webui')
api = Api(app)
CORS(app)
port = 5265

if __name__ == '__main__':
    if sys.argv.__len__() > 1:
        port = sys.argv[1]
    print("Api running on port : {} ".format(port))
    app.run(host="0.0.0.0", port=port, debug=True)