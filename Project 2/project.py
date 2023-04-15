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


def createPlan(query_plan, query):
    clauseDict = getClause(query)
    plan = Plan(clauseDict)
    plan.initialisePlans(query_plan)  # From explain.py
    printPlanDetails(plan)

    return plan


def printPlanDetails(plan):
    print("Plan:", plan)
    fullAnnotation = plan.getPlanAnnotations()
    print("Total Cost: ", plan.totalCost)
    print("All operations: ", plan.operations)
    print("All steps:", fullAnnotation)
    print("Number of steps:", len(plan.operations))
    print("Total Cost:", plan.totalCost)
    print()


def testDummyQuery(connection):

    q1 = "select l_returnflag, l_linestatus, sum(l_quantity) as sum_qty, sum(l_extendedprice) as sum_base_price, sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, avg(l_quantity) as avg_qty, avg(l_extendedprice) as avg_price, avg(l_discount) as avg_disc, count(*) as count_order from lineitem where l_extendedprice > 100 group by l_returnflag, l_linestatus order by l_returnflag, l_linestatus;"
    q2 = "select ps_partkey, sum(ps_supplycost * ps_availqty) as value from partsupp, supplier, nation where ps_suppkey = s_suppkey and s_nationkey = n_nationkey and n_name = 'GERMANY' and ps_supplycost > 20 and s_acctbal > 10 group by ps_partkey having sum(ps_supplycost * ps_availqty) > ( select sum(ps_supplycost * ps_availqty) * 0.0001000000 from partsupp, supplier, nation where ps_suppkey = s_suppkey and s_nationkey = n_nationkey and n_name = 'GERMANY' ) order by value desc;"
    query_3 = "select o_orderpriority, count(*) as order_count from orders where o_totalprice > 100 and exists ( select * from lineitem where l_orderkey = o_orderkey and l_extendedprice > 100 ) group by o_orderpriority order by o_orderpriority;"

    print("For FIRST Query: \n")
    # q1 = "select * from customer C, orders O where C.c_custkey = O.o_custkey;"
    # q2 = "select * from customer C, orders O where C.c_custkey = O.o_custkey and C.c_name like '%23';"
    # Plan object - first Query
    query_plan = connection.generatePlan(q1, constraints=None)
    firstPlan = createPlan(query_plan, q1)

    print("For Second Query: \n")

    # Plan object - second Query
    query_plan = connection.generatePlan(q2, constraints=None)
    secondPlan = createPlan(query_plan, q2)

    comparePlans(firstPlan, secondPlan)


def getClause(query):
    diffClauses = ['where', 'and', 'having']
    variableStr = query.split("from")[0]
    restStr = query.split("from")[-1]
    if "*" in variableStr:
        numVariables = 1000
    else:
        numVariables = len(variableStr.split(","))

    numWhere = restStr.count('where')
    numAnd = restStr.count('and')
    numHaving = restStr.count('having')
    numLimit = restStr.count('limit')

    return {'numProjected': numVariables, 'where': numWhere, 'and': numAnd, 'having': numHaving, 'limit': numLimit}


def compareQueries(plan1, plan2):
    # If plan 1 has a total cost of
    if plan1.totalCost < plan2.totalCost:
        # If plan 1 takes less steps then plan 2
        if len(plan1.operations) < len(plan1.operations):
            pass


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

    # connectMetadata = interface.GUI().initialise_GUI()
    # if connectMetadata is not None:
    #     host, port, database, username, password = connectMetadata
    #     SQL_connect = setupConnection(host, port, database, username, password)
    #     interface.GUI().main_window(SQL_connect)
    connection = setupConnection(
        "localhost", 5432, "TPC-H", "postgres", 922858)
    testDummyQuery(connection)
