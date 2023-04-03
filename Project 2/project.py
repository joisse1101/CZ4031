import csv
import psycopg2
from connection import *

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


if __name__ == '__main__':
    # Metadata to initialise connection with the POSTGRESQL
    # Port Number 5432 by Default
    conn = None

    print("'Connecting to the PostgreSQL database...")
    # Create variables for connection

    conn = psycopg2.connect(
        host="localhost",
        database="TPC-H",
        user="postgres",
        password="922858")

    connection = Connection(conn)
    print("SUCCESS: Connected established.")

    dummyQuery = "select l_returnflag, l_linestatus, sum(l_quantity) as sum_qty, sum(l_extendedprice) as sum_base_price, sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, avg(l_quantity) as avg_qty, avg(l_extendedprice) as avg_price, avg(l_discount) as avg_disc, count(*) as count_order from lineitem where l_extendedprice > 100 group by l_returnflag, l_linestatus order by l_returnflag, l_linestatus;"
    connection.generatePlan(dummyQuery)


