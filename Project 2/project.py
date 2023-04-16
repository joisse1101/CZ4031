import csv
import psycopg2
from explain import *
import json


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


class Connection():

    # Establish connections
    def __init__(self, conn):
        self.conn = conn
        self.cursor = conn.cursor()
        # Constraints
        self.constraints = {
            "enable_bitmapscan": True,
            "enable_hashagg": True,
            "enable_hashjoin": True,
            "enable_indexscan": True,
            "enable_indexonlyscan": True,
            "enable_material": True,
            "enable_mergejoin": True,
            "enable_nestloop": True,
            "enable_seqscan": True,
            "enable_sort": True,
            "enable_tidscan": True,
            "enable_gathermerge": True,
        }

        # Format set to json
        self.query_plan = None

    def generatePlan(self, givenQuery, constraints):
        if givenQuery != None:
            # Generate plans ---------------------------------
            cnt = 0
            self.generateQueryPlan(givenQuery, constraints)
            return self.query_plan

    def generateQueryPlan(self, givenQuery, updated_constr=None):

        # Settle the constraints ----------------------------
        if updated_constr != None:
            cnt = 0
            for x in self.constraints:
                self.constraints[x] = updated_constr[cnt]
                cnt += 1

        constraintStr = ''

        for key, value in self.constraints.items():
            if value == True:
                constraintStr += f"Set {key} to on;"
            else:
                constraintStr += f"Set {key} to off;"

        queryStr = f'{constraintStr} SET max_parallel_workers_per_gather = 0; EXPLAIN (VERBOSE, FORMAT JSON) ' + \
            givenQuery

        # Get the basic query plan
        self.query_plan = self.executeQuery(queryStr)
        self.saveJSON(self.query_plan)

    def saveJSON(self, dictionary):
        # Serializing json
        json_object = json.dumps(dictionary, indent=4)

        # Writing to sample.json
        with open("sample.json", "w") as outfile:
            outfile.write(json_object)

    def executeQuery(self, query):
        try:
            self.cursor.execute(query)
            self.conn.commit()

            # Fetches all the rows of the query results
            plan = self.cursor.fetchall()
            return plan[0][0][0]["Plan"]

        except Exception as ex:
            # ROLLBACK rolls back the current transaction
            self.conn.rollback()
            return None
