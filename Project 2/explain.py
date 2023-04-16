import json
import tkinter as tk
import Pmw
from project import *

RECT_WIDTH = 200
RECT_HEIGHT = 60
CANVAS_WIDTH = 1000
CANVAS_HEIGHT = 1000


# Class that converts query plan into natural language -- ENGLISH
class Plan:
    def __init__(self, clauseDict):
        self.root = None
        self.operations = []  # All operations in the plan
        self.annotationList = []  # Annotation for the steps with labelling
        self.information = []  # Annotation for the steps
        self.all_nodes = []  # All node objects
        self.totalCost = 0  # Total cost of entire plan
        self.visual_to_node = {}

        self.scanOps = None  # List to note the SCAN operations of entire plan
        self.joinOps = None  # List to note the JOIN operations of entire plan
        self.otherOps = None  # List to note the OTHER operations of entire plan
        self.clauseDict = clauseDict  # Dictionary to document the Clauses in query

    # Traverses through the plan tree and setup nodes
    def traversePlans(self, cur, plan):
        # There is children in the nodes
        self.all_nodes.append(cur)
        self.totalCost += cur.cost
        if 'Plans' in plan:
            if len(plan["Plans"]) == 1:
                x1 = cur.x1
                x2 = cur.x2
                y1 = cur.y2 + RECT_HEIGHT / 2
                y2 = y1 + RECT_HEIGHT

                for childPlan in plan["Plans"]:

                    childNode = Node(x1, x2, y1, y2)
                    childNode.setupNode(plan=childPlan, child=False)
                    self.operations.append(childNode.nodeType)
                    cur.children.append(childNode)
                    self.traversePlans(childNode, childPlan)
            else:
                count = 0
                for childPlan in plan["Plans"]:
                    x2 = cur.x1 - RECT_WIDTH + count * (4 * RECT_WIDTH)
                    x1 = x2 - RECT_WIDTH
                    y1 = cur.y2 + RECT_HEIGHT
                    y2 = y1 + RECT_HEIGHT
                    childNode = Node(x1, x2, y1, y2)
                    childNode.setupNode(plan=childPlan, child=False)
                    self.operations.append(childNode.nodeType)
                    cur.children.append(childNode)
                    self.traversePlans(childNode, childPlan)
                    count += 1

    # PostOrder Traversal of tree to get correct order for annotation steps
    def getAnnotationOrder(self, root):
        if root:
            if len(root.children) == 1:
                self.getAnnotationOrder(root.children[0])
            elif len(root.children) == 2:
                self.getAnnotationOrder(root.children[0])
                self.getAnnotationOrder(root.children[1])
            self.information.append(root.annotation)

    # Initialised root and runs tranverse plan function to setup child nodes
    def initialisePlans(self, plan):
        self.root = Node(500, 500+RECT_WIDTH, 0, 0 +
                         RECT_HEIGHT)  # Initialise
        self.root.setupNode(plan=plan, child=False)
        self.operations.append(self.root.nodeType)
        self.information.append(self.root.annotation)
        self.traversePlans(self.root, plan)
        # Rearrange the steps taken (First - Last (Root))
        self.reverseOperationList()
        self.getAnnotationOrder(self.root)
        self.getPlanAnnotations()

    # Get annotation steps with number labelling
    def getPlanAnnotations(self):
        count = 1
        for x in range(1, len(self.information)):
            self.annotationList.append(f"{count}. {self.information[x]}\n")
            count += 1

    def reverseOperationList(self):
        self.operations.reverse()

    # Draws the tree on UI canvas
    def draw(self, query_plan, canvas):

        # Setup nodes...
        self.initialisePlans(query_plan)

        # creation of the rectangle

        uniq_coord = []
        for item in self.all_nodes:
            coord = (item.x1, item.x2, item.y1, item.y2)
            if coord not in uniq_coord:
                uniq_coord.append(coord)
            else:
                item.x1 += 3*RECT_WIDTH/2
                item.x2 += 3*RECT_WIDTH/2
                item.center = (((item.x1 + item.x2)/2), ((item.y1+item.y2)/2))
                new_coord = (item.x1, item.x2, item.y1, item.y2)
                uniq_coord.append(new_coord)

        for item in self.all_nodes:
            x1 = item.x1
            x2 = item.x2
            y1 = item.y1
            y2 = item.y2
            rect = canvas.create_rectangle(x1, y1, x2, y2)

            balloon = Pmw.Balloon()
            balloon.tagbind(canvas, rect, item.information)
            self.visual_to_node[rect] = item

        for item in self.all_nodes:
            gui_text = canvas.create_text(
                (item.center[0], item.center[1]), text=item.nodeType)
            self.visual_to_node[gui_text] = item

        for item in self.all_nodes:
            for child in item.children:
                canvas.create_line(child.center[0], child.center[1] - RECT_HEIGHT/2, item.center[0],
                                   item.center[1] + RECT_HEIGHT/2, arrow=tk.LAST)


class Node:

    def __init__(self, x1, x2, y1, y2):

        # For tree crafting
        self.x1 = x1
        self.x2 = x2
        self.y1 = y1
        self.y2 = y2
        self.center = ((x1+x2)/2, (y1+y2)/2)

        self.nodeType = None
        self.cost = None  # Cost
        self.rows = None
        self.width = None  # Plan width (Same for nodes under same query)

        self.children = []  # Children of the node (Empty if no children)
        self.information = []  # Natural Language: Description for each node

    # Set up basic variables for the nodes
    def setupNode(self, plan, child):

        # Set up basics
        self.nodeType = plan['Node Type']
        self.cost = plan['Total Cost']
        self.rows = plan['Plan Rows']
        self._isChildNode = child

        self.annotatePlan(plan)

    # Get the annotation for current node
    def annotatePlan(self, plan):

        # SORT --------------------------------------------------------------------------------------
        if self.nodeType == "Sort":
            # Attr used to Sort
            attr = ",".join(plan['Sort Key'])
            self.annotation = f"{self.nodeType} operation was performed. Rows are sorted to be in order based on key(s): ({attr})"

        # AGGREGATE -----------------------------------------------------------------------------------
        elif self.nodeType == "Aggregate":
            if 'Group Key' in plan:
                if len(plan['Group Key']) > 1:
                    attr = ",".join(plan['Group Key'])
                else:
                    attr = plan['Group Key']

                self.annotation = f"{self.nodeType} operation was performed on key(s): ({attr})"
            else:
                self.annotation = f"{self.nodeType} operation was performed."

        # SEQ SCAN -----------------------------------------------------------------------------------
        elif self.nodeType == "Seq Scan":
            relation = plan['Relation Name']
            self.annotation = f"Sequential Scan operation was performed."

            if 'Filter' in plan:
                self.annnotation = self.annotation[:-1]
                self.annotation += f"with filtering condition: {plan['Filter']} on relation {relation}.\n Reason: The selectivity condition allows allow for lower cost and lesser row returns."
            else:
                self.annotation += "\n Reason: The operation allows for lesser return of rows. "

        # INDEX SCAN -----------------------------------------------------------------------------------
        elif self.nodeType == "Index Scan":
            index = plan['Index Name']
            relation = plan['Relation Name']

            self.annotation = f"{self.nodeType} operation was performed on index: ({index}) on relation {relation}."
            if 'Filter' in plan:
                self.annnotation = self.annotation[:-1]
                self.annotation += f"with filtering condition: {plan['Filter']} "

        # HASH JOIN or MERGE JOIN -----------------------------------------------------------------------------------
        elif self.nodeType == "Hash Join" or self.nodeType == "Merge Join":
            cond = self.nodeType.split(" ")[0] + " Cond"
            self.annotation = f"{self.nodeType} operation was performed with condition: {plan[cond]}."

        # NESTED LOOP --------------------------------------------------------------------------
        elif self.nodeType == "Nested Loop":
            self.annotation = f"{self.nodeType} join operation was performed ({plan['Join Type']})."

        # BITMAP SCAN -----------------------------------------------------------------------------------------------
        elif self.nodeType == "Bitmap Heap Scan":
            table = plan['Relation Name']
            self.annotation = f"{self.nodeType} operation was performed on table:{table} "

         # BITMAP INDEX SCAN -----------------------------------------------------------------------------------------------
        elif self.nodeType == "Bitmap Index Scan":
            index = plan['Index Name']
            cond = plan['Index Cond']
            self.annotation = f"{self.nodeType} operation was performed with index:{index} and condition: {cond}"

        elif self.nodeType == "Limit":
            self.annotation = f"{self.nodeType} operation was performed."

        elif self.nodeType == "Materialise":
            self.annotation = f"{self.nodeType} operation was performed, where the output of child operations is materalized to memory before upper node is executed."

        else:
            self.annotation = f"{self.nodeType} operation was performed."

# Compare the 2 query plans based on COST, SCAN operations, JOIN operations and OTHER operations


def comparePlans(p1, p2):
    description = ""
    description += checkCost(p1, p2)
    p1.scanOps, p1.joinOps, p1.otherOps = categoriesOperations(p1)
    p2.scanOps, p2.joinOps, p2.otherOps = categoriesOperations(p2)

    # Check if there is an increase/decrease in scan operations and identify the odd one out
    description += "\n<-- Analyzing SCAN operation changes...-->\n"
    description += checkScan(p1, p2)

    description += "\n<-- Analyzing JOIN operation changes...-->\n"
    description += checkJoin(p1, p2)

    description += "\n<-- Analyzing OTHER operation changes...-->\n"
    description += checkOther(p1, p2)

    return description

# Compares COST of 2 queries


def checkCost(p1, p2):
    # No additional steps is taken... and there might only be a change in plan
    description = ""
    if p1.totalCost < p2.totalCost:
        description += f"Query 2's plan has increased in cost by {p2.totalCost- p1.totalCost}.\n"
        description += "This might be because of the size of data rows returned during processing\n "

        if p1.clauseDict["where"] >= 1 and p2.clauseDict["where"] == 0:
            description += "due to the removal of WHERE clause in Query 2 where there are more rows to be returned. \n"
        elif p1.clauseDict["where"] >= 1 and p2.clauseDict["where"] >= 1:
            if p1.clauseDict["and"] > p2.clauseDict["and"]:
                description += "due to removing the number of conditions in Query 2. \n"
        if p1.clauseDict["having"] >= 1 and p2.clauseDict["having"] == 0:
            description += "due to the removal HAVING clause in Query 2 where there are more rows to be returned. \n"

    elif p1.totalCost > p2.totalCost:
        description += f"Query 2's plan has decreased in cost by {p1.totalCost- p2.totalCost}.\n"
        description += "This might be because of the size of data rows returned during processing "

        # Comparing number of WHERE/ AND / HAVING clause
        if p1.clauseDict["where"] == 0 and p2.clauseDict["where"] >= 1:
            description += "due to the additon of a WHERE clause in Query 2 where the condition allows for lesser rows to be processed. \n"
        elif p1.clauseDict["where"] >= 1 and p2.clauseDict["where"] >= 1:
            if p1.clauseDict["and"] < p2.clauseDict["and"]:
                description += "due to having more conditions in Query 2. \n"
        if p1.clauseDict["having"] == 0 and p2.clauseDict["having"] >= 1:
            description += "due to the addition of a HAVING clause in Query 2 where the condition allows for lesser rows to be processed. \n"

    else:
        description += f"Query 2's plan cost is the same as Query Plan 1.\n"

    return description

# Compare SCAN operations of 2 queries


def checkScan(p1, p2):
    description = ""
    reason = ""
    if p1.scanOps == p2.scanOps:
        description += "There has been no change for the scan operations.\n"
    else:
        # No change in the length but just the value
        if len(p1.scanOps) == len(p2.scanOps):
            for x in range(len(p1.scanOps)):
                if p1.scanOps[x] != p2.scanOps[x]:
                    description += f"SCAN operation: {p1.scanOps[x]} has been switched out to {p2.scanOps[x]}.\n"
                    reason = getSwitch(p1.scanOps[x], p2.scanOps[x])

        # Change in the length
        else:
            if len(p1.scanOps) < len(p2.scanOps):
                diff = list(set(p2.scanOps) - set(p1.scanOps))
                if len(diff) != 0:
                    description += f"There is an introduction of {len(diff)} SCAN operations: {','.join(diff)}\n"
                    reason = getDiff(diff, "insert")

            else:
                diff = list(set(p1.scanOps) - set(p2.scanOps))
                if len(diff) != 0:
                    description += f"There is an removal of {len(diff)} SCAN operations: {','.join(diff)}\n"
                    reason = getDiff(diff, "remove")

            if reason == "":
                reason = "There is no new SCAN operation.\n"

    description += reason

    return description

# Compare JOIN operations of 2 queries


def checkJoin(p1, p2):

    description = ""
    reason = ""

    if p1.joinOps == p2.joinOps:
        description += "There has been no change for the join operations.\n"
    else:
        # No change in the length but just the value
        if len(p1.joinOps) == len(p2.joinOps):
            for x in range(len(p1.joinOps)):
                if p1.joinOps[x] != p2.joinOps[x]:
                    print(
                        f"P1 join operation: {p1.joinOps[x]} and {p2.joinOps[x]}")
                    description += f"JOIN operation: {p1.joinOps[x]} has been switched out to {p2.joinOps[x]}.\n"
                    reason = getSwitch(p1.joinOps[x], p2.joinOps[x])

        # Change in the length
        else:
            if len(p1.joinOps) < len(p2.joinOps):
                diff = list(set(p2.joinOps) - set(p1.joinOps))
                if len(diff) != 0:
                    description += f"There is an introduction of {len(diff)} JOIN operations: {','.join(diff)}\n"
                    reason = getDiff(diff, "insert")

            else:
                diff = list(set(p1.joinOps) - set(p2.joinOps))
                if len(diff) != 0:
                    description += f"There is an removal of {len(diff)} JOIN operations: {','.join(diff)}\n"
                    reason = getDiff(diff, "remove")

            if reason == "":
                reason = "There is no new JOIN operation.\n"

    description += reason

    return description

# Compare OTHER operations of 2 queries


def checkOther(p1, p2):
    description = ""
    reason = ""
    if p1.otherOps == p2.otherOps:
        description += "There has been no change for the other operations.\n"
    else:
        # No change in the length but just the value
        if len(p1.otherOps) == len(p2.otherOps):
            for x in range(len(p1.otherOps)):
                if p1.otherOps[x] != p2.otherOps[x]:
                    description += f"OTHER operation: {p1.otherOps[x]} has been switched out to {p2.otherOps[x]}.\n"
                    reason = getSwitch(p1.otherOps[x], p2.otherOps[x])

        # Change in the length
        else:
            if len(p1.otherOps) < len(p2.otherOps):
                diff = list(set(p2.otherOps) - set(p1.otherOps))
                description += f"There is an introduction of {len(diff)} OTHER operations: {','.join(diff)}\n"
                reason = getDiff(diff, "insert")

            else:
                diff = list(set(p1.otherOps) - set(p2.otherOps))
                description += f"There is an removal of {len(diff)} OTHER operations: {','.join(diff)}\n"
                reason = getDiff(diff, "remove")
            if reason == "":
                reason = "There is no new OTHER operation.\n"

    description += reason

    return description

# Seperates all operations into SCAN, JOIN and OTHER


def categoriesOperations(p):
    # Scan operations
    scanOperation = []
    joinOperation = []
    otherOperation = []

    for x in range(len(p.operations)):
        # Operation is a scan operation
        if p.operations[x] in ['Seq Scan', 'Index Scan', 'Bitmap Heap Scan', 'Bitmap Index Scan', 'CTE Scan']:
            scanOperation.append(p.operations[x])
        elif p.operations[x] in ['Nested Loop', 'Hash Join', 'Merge Join']:
            joinOperation.append(p.operations[x])
        else:
            otherOperation.append(p.operations[x])

    return scanOperation, joinOperation, otherOperation


# If Operations are switched..
def getSwitch(p1Ops, p2Ops):
    # https://www.sqlshack.com/internals-of-physical-join-operators-nested-loops-join-hash-match-join-merge-join-in-sql-server/#:~:text=Nested%20Loops%20are%20used%20to,table%20to%20join%20equi%20joins.
    reason = ""
    print("OPERATIONS COMPARED", p1Ops, "and", p2Ops)
    # JOIN OPERATIONS
    if p1Ops == "Nested Loop":
        if p2Ops == "Hash Join":
            reason = "This is likely because Query 2 has larger, sorted and non-indexed data, hence making it more efficient than the Nested Loop Join."
        elif p2Ops == "Merge Join":
            reason = "This is likely because the join columns are indexed or sorted in Query 2, hence making it more efficient than the Nested Loop Join."
    elif p1Ops == "Hash Join":
        if p2Ops == "Nested Loop":
            reason = "This is likely because Query 2 has smaller transactions and smaller data, hence making it more efficient than the Hash Join."
        elif p2Ops == "Merge Join":
            reason = "This is likely because the join columns are indexed or sorted in Query 2, hence making it more efficient than the Hash Join."
    elif p1Ops == "Merge Join":
        if p2Ops == "Nested Loop":
            reason = "This is likely because Query 2 has smaller, non-sorted and non-indexed data, hence making it more efficient than the Merge Join."
        elif p2Ops == "Hash Join":
            reason = "This is likely because Query 2 has sorted but non-indexed data, hence making it more efficient than the Merge Join."
    # Scan Operations
    elif p1Ops == "Index Scan":
        if p2Ops == "Seq Scan":
            reason = "This is likely because there is no index available on key, and majority of the rows are getting fetched for Query 2."
        elif p2Ops == "Bitmap Heap Scan":
            reason = "This is likely because there are multiple indexes over the same table, and a combination is needed in order to minimize the number of selected rows."
        elif p2Ops == "Bitmap Index Scan":
            reason = "This is likely because there are multiple indexes over the same table, and a combination is needed in order to minimize the number of selected rows."
        elif p2Ops == "CTE Scan":
            reason = "This is likely because a WITH clause is involved in Query 2."

    elif p1Ops == "Seq Scan":
        if p2Ops == "Index Scan":
            reason = "This is likely because there is an index available on key, and only a small handful of rows are getting fetched for Query 2. "
        elif p2Ops == "Bitmap Heap Scan":
            reason = "This is likely because there are multiple indexes over the same table, and a combination is needed in order to minimize the number of selected rows."
        elif p2Ops == "Bitmap Index Scan":
            reason = "This is likely because there are multiple indexes over the same table, and a combination is needed in order to minimize the number of selected rows."
        elif p2Ops == "CTE Scan":
            reason = "This is likely because a WITH clause is involved in Query 2."

    elif p1Ops == "Bitmap Heap Scan":
        if p2Ops == "Index Scan":
            reason = "This is likely because only a small handful of rows are getting fetched for Query 2. Index Scan is also useful in combination with the LIMIT clause."
        elif p2Ops == "Seq Scan":
            reason = "This is likely because there is no index available on key, and majority of the rows are getting fetched for Query 2."
        elif p2Ops == "Bitmap Index Scan":
            reason = "This is likely because the table being queried has a larger number of rows."
        elif p2Ops == "CTE Scan":
            reason = "This is likely because a WITH clause is involved in Query 2."

    elif p1Ops == "Bitmap Index Scan":
        if p2Ops == "Index Scan":
            reason = "This is likely because only a small handful of rows are getting fetched for Query 2. Index Scan is also useful in combination with the LIMIT clause."
        elif p2Ops == "Seq Scan":
            reason = "This is likely because there is no index available on key, and majority of the rows are getting fetched for Query 2."
        elif p2Ops == "Bitmap Heap Scan":
            reason = "This is likely because the table being queried has a smaller number of rows."
        elif p2Ops == "CTE Scan":
            reason = "This is likely because a WITH clause is involved in Query 2."

    elif p1Ops == "CTE Scan":
        if p2Ops == "Index Scan":
            reason = "This is likely because there is an index available on key, and only a small handful of rows are getting fetched for Query 2."
        elif p2Ops == "Seq Scan":
            reason = "This is likely because there is no index available on key, and majority of the rows are getting fetched for Query 2."
        elif p2Ops == "Bitmap Heap Scan":
            reason = "This is likely because there are multiple indexes over the same table, and a combination is needed in order to minimize the number of selected rows."
        elif p2Ops == "Bitmap Index Scan":
            reason = "This is likely because there are multiple indexes over the same table, and a combination is needed in order to minimize the number of selected rows."
    else:
        reason = ""
    reason += "\n"
    return reason

# If there is a introduction/removal of operations


def getDiff(diffList, status):
    # Remove operations
    opsAnnotation = ""
    if status == "remove":
        for x in diffList:
            opsAnnotation += removeAnnotation(x)

    # Insert Operations
    elif status == "insert":
        for x in diffList:
            opsAnnotation += insertAnnotation(x)

    return opsAnnotation

# Annotations for insertion


def insertAnnotation(x):
    # Scan Operations
    if x == "Index Scan":
        reason = f"Reason for {x}: As the table may be indexed and involves sorting."

    elif x == "Seq Scan":
        reason = f"Reason for {x}: As the table may not be indexed and involves sorting."

    elif x == "Bitmap Heap Scan":
        reason = f"Reason for {x}: As the join conditions involve low-cardinality columns."

    elif x == "Bitmap Index Scan":
        reason = f"Reason for {x}: As the table being queried has a large number of rows and the column being indexed has a low cardinality."
    # Join Operations

    elif x == "Nested Loop":
        reason = f"Reason for {x}: As joining of relations, likely with smaller data, is involved in Query 2. "

    elif x == "Hash Join":
        reason = f"Reason for {x}: As joining of relations, likely with larger, sorted and non-indexed data, is involved in Query 2.\n"

    elif x == "Merge Join":
        reason = f"Reason for {x}: As joining of relations, likely with larger, sorted and/or indexed data, is involved in Query 2."

    # Other Operations
    elif x == "Aggregate":
        reason = f"Reason for {x}: As calculations on multiple values that returns a single value is involved in Query 2.\n"

    elif x == "Limit":
        reason = f"Reason for {x}: As Query 2 has the LIMIT clause, where the number of records to return is specified.\n"

    elif x == "Memoize":
        reason = f"Reason for {x}: As the Query 2 was optimized by using temporary tables or table variables to store the results of a query.\n"

    else:
        reason = "-"

    return reason

# Annotations for removal


def removeAnnotation(x):

    # Scan Operations
    if x == "Index Scan":
        reason = f"Reason for {x}: As the table may no longer be indexed or need sorting.\n"

    elif x == "Seq Scan":
        reason = f"Reason for {x}: As the table is be indexed and does not involve sorting.\n"

    elif x == "Bitmap Heap Scan":
        reason = f"Reason for {x}: As the join conditions no longer involve low-cardinality columns.\n"

    elif x == "Bitmap Index Scan":
        reason = f"Reason for {x}: As the column being indexed no longer has a low cardinality.\n"
    # Join Operations

    elif x == "Nested Loop":
        reason = f"Reason for {x}: As Query 2 no longer involve an outer join.\n "

    elif x == "Hash Join":
        reason = f"Reason for {x}: As the Query 2 no longer involves a join key.\n"

    # Other Operations
    elif x == "Aggregate":
        reason = f"Reason for {x}: As there are no longer calculations on multiple values that returns a single value.\n"

    elif x == "Limit":
        reason = f"Reason for {x}: As Query 2 no longer specifies the number of records to return.\n"

    elif x == "Memoize":
        reason = f"Reason for {x}: As the Query 2 can no longer be optimized by using temporary tables or table variables.\n"

    else:
        reason = "-"

    return reason
