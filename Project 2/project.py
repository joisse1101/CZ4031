import csv
import psycopg2
from connection import *
from explain import *


import interface


# Process the csv files to remove extra"|"


def processCSVFile():
    tbl_list = ['customer', 'lineitem', 'nation',
                'part', 'partsupp', 'region', 'supplier', 'orders']
    for x in range(len(tbl_list)):
        with open(f'tool/dbgen/csv/{tbl_list[x]}.csv', 'r') as f:
            lines = []
            for line in f:
                line = line[:-2]
                lines.append(line)
        with open(f'tool/dbgen/csv/new/{tbl_list[x]}.csv', 'w') as w:
            for line in lines:
                w.write(line)
                w.write('\n')


def setupConnection(host, port, database, username, password):
    # Metadata to initialise connection with the POSTGRESQL
    # Port Number 5432 by Default
    conn = None

    print("'Connecting to the PostgreSQL database...")
    # Create variables for connection

    conn = psycopg2.connect(
        host=host,
        port=port,
        database=database,
        user=username,
        password=password)

    connection = Connection(conn)
    print("SUCCESS: Connected established.")
    return connection


if __name__ == '__main__':

    connectMetadata = interface.GUI().initialise_GUI()
    if connectMetadata is not None:
        host, port, database, username, password = connectMetadata
        SQL_connect = setupConnection(host, port, database, username, password)
        interface.GUI().main_window(SQL_connect)
    # connection = setupConnection(
    #     "localhost", 5432, "TPC-H", "postgres", 922858)
    # testDummyQuery(connection)
