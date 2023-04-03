class Connection():

    # Establish connections
    def __init__(self, conn):
        self.conn = conn
        self.cursor = conn.cursor()
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



    def generatePlan(self, givenQuery):
        if givenQuery != None:
            # Generate plans ---------------------------------
            self.generateQueryPlan(givenQuery)
            # self.generateAlternative(givenQuery)

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
        print("The query string : ", queryStr)
        generatedPlan = self.executeQuery(queryStr)
        print(generatedPlan)

    def executeQuery(self, query):
        try:
            self.cursor.execute(query)
            self.conn.commit()

            # Fetches all the rows of the query results
            plan = self.cursor.fetchall()
            return plan[0][0][0]["Plan"]

        except Exception as ex:
            self.conn.rollback()
            raise Exception(ex)
