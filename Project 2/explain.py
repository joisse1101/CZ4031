import json
import tkinter as tk
import Pmw
from project import *

RECT_WIDTH = 200
RECT_HEIGHT = 60
CANVAS_WIDTH = 1000
CANVAS_HEIGHT = 1000

all_nodes = []
visual_to_node = {}


# Class that converts query plan into natural language -- ENGLISH
class Plan():
    def __init__(self, clauseDict):
        self.root = None
        self.q1_info = None
        self.q2_info = None
        self.operations = []

        self.information = []  # Combined Information

        self.totalCost = None

        # With steps
        self.annotationList = []

        # Clause list that documents the number of [VARIABLES IN PROJECTED, WHERE, AND, HAVING]
        self.clauseDict = clauseDict
        #
        self.scanOps = None
        self.joinOps = None
        self.otherOps = None

    def traversePlans(self, cur, plan):
        # There is children in the nodes
        if 'Plans' in plan:
            for childPlan in plan["Plans"]:

                childNode = Node(1, 1, 1, 1)
                childNode.setupNode(plan=childPlan, child=False)
                self.operations.append(childNode.nodeType)
                self.information.append(childNode.annotation)
                cur.children.append(childNode)
                self.traversePlans(childNode, childPlan)

    def initialisePlans(self, plan):
        self.root = Node(1, 1, 1, 1)  # Initialise
        self.root.setupNode(plan=plan, child=False)
        self.totalCost = self.root.cost
        self.operations.append(self.root.nodeType)
        self.information.append(self.root.annotation)
        self.traversePlans(self.root, plan)
        # Rearrange the steps taken (First - Last (Root))
        self.reverseOperationList()

    def getPlanAnnotations(self):
        count = 1
        for x in range(len(self.information)-1, -1, -1):
            self.annotationList.append(f"{count}. {self.information[x]}\n")
            count += 1

    def reverseOperationList(self):
        self.operations.reverse()
        self.information.reverse()


class Node():

    def __init__(self, x1, x2, y1, y2):

        self.x1 = x1
        self.x2 = x2
        self.y1 = y1
        self.y2 = y2

        self.nodeType = None
        self.strategy = None
        self.cost = None  # Cost
        # Number of rows returned (Same for nodes under same query)
        self.rows = None
        self.width = None  # Plan width (Same for nodes under same query)
        self.output = None  # Output

        self.children = []  # Children of the node (Empty if no children)

        # Natural Language: Description for each node
        self.information = []

    def setupNode(self, plan, child):

        # Set up basics

        self.nodeType = plan['Node Type']
        self.cost = plan['Total Cost']
        self.rows = plan['Plan Rows']
        self._isChildNode = child

        self.annotatePlan(plan)

    def annotatePlan(self, plan):

        # SORT --------------------------------------------------------------------------------------
        if self.nodeType == "Sort":
            # Attr used to Sort
            attr = ",".join(plan['Sort Key'])
            self.annotation = f"italics{self.nodeType}) operation was performed. Rows are sorted to be in order based on key(s): (<b>{attr}</b>)"

        # AGGREGATE -----------------------------------------------------------------------------------
        elif self.nodeType == "Aggregate":
            if 'Group Key' in plan:
                if len(plan['Group Key']) > 1:
                    attr = ",".join(plan['Group Key'])
                else:
                    attr = plan['Group Key']

                self.annotation = f"<b>{self.nodeType}</b> operation was performed on key(s): (<b>{attr}</b>)"
            else:
                self.annotation = f"<b>{self.nodeType}</b> operation was performed."

        # SEQ SCAN -----------------------------------------------------------------------------------
        elif self.nodeType == "Seq Scan":
            relation = plan['Relation Name']
            self.annotation = f"<b>Sequential Scan</b> operation was performed."

            if 'Filter' in plan:
                self.annnotation = self.annotation[:-1]
                self.annotation += f"with filtering condition: {plan['Filter']} on relation <b>{relation}</b>.\n \
                    Reason: The selectivity condition allows allow for lower cost and lesser row returns."
            else:
                self.annotation += "\n Reason: The operation allows for lesser return of rows. "

        # INDEX SCAN -----------------------------------------------------------------------------------
        elif self.nodeType == "Index Scan":
            index = plan['Index Name']
            relation = plan['Relation Name']

            self.annotation = f"<b>{self.nodeType}</b> operation was performed on index: (<b>{index}</b>) on relation <b>{relation}</b>."
            if 'Filter' in plan:
                self.annnotation = self.annotation[:-1]
                self.annotation += f"with filtering condition: {plan['Filter']} "

        # HASH JOIN or MERGE JOIN -----------------------------------------------------------------------------------
        elif self.nodeType == "Hash Join" or self.nodeType == "Merge Join":
            cond = self.nodeType.split(" ")[0] + " Cond"
            self.annotation = f"<b>{self.nodeType}</b> operation was performed with condition: {plan[cond]}."

        # NESTED LOOP --------------------------------------------------------------------------
        elif self.nodeType == "Nested Loop":
            self.annotation = f"<b>{self.nodeType} join</b> operation was performed ({plan['Join Type']})."

        # BITMAP SCAN -----------------------------------------------------------------------------------------------
        elif self.nodeType == "Bitmap Heap Scan":
            table = plan['Relation Name']
            self.annotation = f"<b>{self.nodeType}</b> operation was performed on table:{table} "

         # BITMAP INDEX SCAN -----------------------------------------------------------------------------------------------
        elif self.nodeType == "Bitmap Index Scan":
            index = plan['Index Name']
            cond = plan['Index Cond']
            self.annotation = f"<b>{self.nodeType}</b> operation was performed with index:{index} and condition: {cond}"

        elif self.nodeType == "Limit":
            self.annotation = f"<b>{self.nodeType}</b> operation was performed."

        elif self.nodeType == "Materialise":
            self.annotation = f"<b>{self.nodeType}</b> operation was performed, where the output of child operations is materalized to memory before upper node is executed."

        else:
            self.annotation = f"<b>{self.nodeType}</b> operation was performed."

    def draw(query_plan, canvas):
        data = json.loads(query_plan)
        all_nodes.clear()

        root_op = Plan(500, 500+RECT_WIDTH, 0, 0+RECT_HEIGHT, '', '')
        build_plan = (root_op, data)

        # creation of the rectangle

        uniq_coord = []
        for item in all_nodes:
            coord = (item.x1, item.x2, item.y1, item.y2)
            if coord not in uniq_coord:
                uniq_coord.append(coord)
            else:
                item.x1 += 3*RECT_WIDTH/2
                item.x2 += 3*RECT_WIDTH/2
                item.center = (((item.x1 + item.x2)/2), ((item.y1+item.y2)/2))
                new_coord = (item.x1, item.x2, item.y1, item.y2)
                uniq_coord.append(new_coord)

        for item in all_nodes:
            x1 = item.x1
            x2 = item.x2
            y1 = item.y1
            y2 = item.y2
            rect = canvas.create_rectangle(x1, y1, x2, y2)

            balloon = Pmw.Balloon()
            balloon.tagbind(canvas, rect, item.information)
            visual_to_node[rect] = item

        for item in all_nodes:
            gui_text = canvas.create_text(
                (item.center[0], item.center[1]), text=item.operation)
            visual_to_node[gui_text] = item

        for item in all_nodes:
            for child in item.children:
                canvas.create_line(child.center[0], child.center[1] - RECT_HEIGHT/2, item.center[0],
                                   item.center[1] + RECT_HEIGHT/2, arrow=tk.LAST)

    if __name__ == '__main__':

        draw("query_plan.json")


def comparePlans(p1, p2):
    # No additional steps is taken... and there might only be a change in plan
    print("Comparing the plans...")
    description = ""
    if p1.totalCost < p2.totalCost:
        description += f"Query 2's plan has increased in cost by {p2.totalCost- p1.totalCost}.\n"
        if p1.clauseDict["where"] == 1 and p2.clauseDict["where"] == 0:
            description += "This is likely due to the removal of WHERE clause in Query 2 as there are more rows to be returned. \n"
        elif p1.clauseDict["where"] == 1 and p2.clauseDict["where"] == 1:
            if p1.clauseDict["and"] > p2.clauseDict["and"]:
                description += "This is likely due to removing the number of conditions in Query 2. \n"
        if p1.clauseDict["having"] == 1 and p2.clauseDict["having"] == 0:
            description += "This is likely due to the removal HAVING clause in Query 2 as there are more rows to be returned. \n"

    elif p1.totalCost > p2.totalCost:
        description += f"Query 2's plan has decreased in cost by {p1.totalCost- p2.totalCost}.\n"
        # If p1 query does not have (WHERE CLAUSE)
        if p1.clauseDict["where"] == 0 and p2.clauseDict["where"] == 1:
            description += "This is likely due to the additon of a WHERE clause in Query 2 as the condition allows for lesser rows to be processed. \n"
        elif p1.clauseDict["where"] == 1 and p2.clauseDict["where"] == 1:
            if p1.clauseDict["and"] < p2.clauseDict["and"]:
                description += "This is likely due to having more conditions in Query 2. \n"
        if p1.clauseDict["having"] == 0 and p2.clauseDict["having"] == 1:
            description += "This is likely due to the addition of a HAVING clause in Query 2 as the condition allows for lesser rows to be processed. \n"

    else:
        description += f"Query 2's plan cost is the same as Query Plan 1.\n"

    p1.scanOps, p1.joinOps, p1.otherOps = categoriesOperations(p1)
    p2.scanOps, p2.joinOps, p2.otherOps = categoriesOperations(p2)

    # Check if there is an increase/decrease in scan operations and identify the odd one out
    description += "\n<-- Analyzing SCAN operation changes...-->\n"
    description += checkScan(p1, p2)

    description += "\n<-- Analyzing JOIN operation changes...-->\n"
    description += checkJoin(p1, p2)

    description += "\n<-- Analyzing OTHER operation changes...-->\n"
    description += checkOther(p1, p2)
    # checkProj(p1, p2)

    print(description)


# def checkProj(p1, p2):
#     print("P1 CLAUSE DICT: ", p1.clauseDict)
#     print("P2 CLAUSE DICT: ", p2.clauseDict)


def checkScan(p1, p2):
    description = ""
    if p1.scanOps == p2.scanOps:
        description += "There has been no change for the scan operations.\n"
    else:
        # No change in the length but just the value
        if len(p1.scanOps) == len(p2.scanOps):
            for x in range(len(p1.scanOps)):
                if p1.scanOps[x] != p2.scanOps[x]:
                    description += f"SCAN operation: {p1.scanOps[x]} has been switched out to {p1.scanOps[x]}.\n"

        # Change in the length
        else:
            if len(p1.scanOps) < len(p2.scanOps):
                diff = list(set(p2.scanOps) - set(p1.scanOps))
                description += f"There is an introduction of {len(p2.scanOps)-len(p1.scanOps)} SCAN operations: {','.join(diff)}\n"

            else:
                diff = list(set(p1.scanOps) - set(p2.scanOps))
                description += f"There is an removal of {len(p1.scanOps)-len(p2.scanOps)} SCAN operations: {','.join(diff)}\n"

    return description


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
                    description += f"JOIN operation: {p1.joinOps[x]} has been switched out to {p2.joinOps[x]}.\n"

        # Change in the length
        else:
            if len(p1.joinOps) < len(p2.joinOps):
                diff = list(set(p2.joinOps) - set(p1.joinOps))
                description += f"There is an introduction of {len(p2.joinOps)-len(p1.joinOps)} JOIN operations: {','.join(diff)}\n"

            else:
                diff = list(set(p1.joinOps) - set(p2.joinOps))
                description += f"There is an removal of {len(p1.joinOps)-len(p2.joinOps)} JOIN operations: {','.join(diff)}\n"

    return description + reason


def checkOther(p1, p2):
    description = ""
    if p1.otherOps == p2.otherOps:
        description += "There has been no change for the other operations.\n"
    else:
        # No change in the length but just the value
        if len(p1.otherOps) == len(p2.otherOps):
            for x in range(len(p1.otherOps)):
                if p1.otherOps[x] != p2.otherOps[x]:
                    description += f"OTHER operation: {p1.otherOps[x]} has been switched out to {p2.otherOps[x]}.\n"

        # Change in the length
        else:
            if len(p1.otherOps) < len(p2.otherOps):
                diff = list(set(p2.otherOps) - set(p1.otherOps))
                # print("List(p1):", p1.otherOps)
                # print("List(p2):", p2.otherOps)
                # print("DIFF LIST: ", diff)
                description += f"There is an introduction of {len(diff)} OTHER operations: {','.join(diff)}\n"
                reason = getDiff(diff, "insert")

            else:
                diff = list(set(p1.otherOps) - set(p2.otherOps))
                description += f"There is an removal of {len(p1.otherOps)-len(p2.otherOps)} OTHER operations: {','.join(diff)}\n"
                reason = getDiff(diff, "remove")

    description += reason

    return description


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
    # JOIN OPERATIONS
    if p1Ops == "Nested Loop":
        if p2Ops == "Hash Join":
            reason = "This is likely because it is more efficent than the nested loop as there are filters used in Query 2."
        elif p2Ops == "Merge Join":
            reason = "This is likely because it is more efficent than the nested loop as the join columns are indexed in Query 2."

    return reason
# Get annotation if there is removal/insertation of operations

# If Operations are introduced...


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


def insertAnnotation(x):
    if x == "Nested Loop":
        reason = ""
    elif x == "Aggregate":
        reason = f"{x}: As calculations on multiple values that returns a single value is involved.\n"
    elif x == "Hash Join":
        reason = f"{x}: As the join involves a join key and is more efficient.\n"
    elif x == "Limit":
        reason = f"{x}: As the Query 2 has the LIMIT clause.\n"
    else:
        reason = ""

    return reason


def removeAnnotation(x):
    if x == "Nested Loop":
        reason = ""

    elif x == "":
        reason = ""
