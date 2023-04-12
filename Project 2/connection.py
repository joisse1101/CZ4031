import json


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

    def generatePlan(self, givenQuery):
        if givenQuery != None:
            # Generate plans ---------------------------------
            self.generateQueryPlan(givenQuery)
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
