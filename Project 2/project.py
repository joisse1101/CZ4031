import csv
import psycopg2
from connection import *
from explain import *

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


def displayPlan(plan):
    plan = Plan()
    plan.initialisePlans(query_plan)  # From explain.py
    fullAnnotation = plan.getPlanAnnotations()
    print("All operations: ", plan.operations)
    print("All steps:", fullAnnotation)
    print("Total Cost:", plan.totalCost)
    return plan


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

    # Will be good if user can get sample/ dummy query
    # QUERY 1
    query_1 = "select l_returnflag, l_linestatus, sum(l_quantity) as sum_qty, sum(l_extendedprice) as sum_base_price, sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, avg(l_quantity) as avg_qty, avg(l_extendedprice) as avg_price, avg(l_discount) as avg_disc, count(*) as count_order from lineitem where l_extendedprice > 100 group by l_returnflag, l_linestatus order by l_returnflag, l_linestatus;"
    # QUERY 2
    query_2 = "select ps_partkey, sum(ps_supplycost * ps_availqty) as value from partsupp, supplier, nation where ps_suppkey = s_suppkey and s_nationkey = n_nationkey and n_name = 'GERMANY' and ps_supplycost > 20 and s_acctbal > 10 group by ps_partkey having sum(ps_supplycost * ps_availqty) > ( select sum(ps_supplycost * ps_availqty) * 0.0001000000 from partsupp, supplier, nation where ps_suppkey = s_suppkey and s_nationkey = n_nationkey and n_name = 'GERMANY' ) order by value desc;"
    query_3 = "select o_orderpriority, count(*) as order_count from orders where o_totalprice > 100 and exists ( select * from lineitem where l_orderkey = o_orderkey and l_extendedprice > 100 ) group by o_orderpriority order by o_orderpriority;"

    query_1 = "select * from customer C, orders O where C.c_custkey = O.o_custkey;"
    query_2 = f"select * from customer C, orders O where C.c_custkey = O.o_custkey and C.c_name like '%23';"

    # First Query =======================================
    print("For FIRST Query: \n")
    query_plan = connection.generatePlan(query_1)
    firstPlan = displayPlan(query_plan)

    # First Query =======================================
    print("For SECOND Query: \n")
    query_plan = connection.generatePlan(query_2)
    secondPlan = displayPlan(query_plan)
