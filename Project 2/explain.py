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
    def __init__(self):
        self.root = None
        self.q1_info = None
        self.q2_info = None
        self.operations = []
        self.information = []  # Combined Information
        self.totalCost = None

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

    def getPlanAnnotations(self):
        count = 1
        fullAnnotation = ""
        for x in range(len(self.information)-1, -1, -1):
            fullAnnotation += f"{count}. {self.information[x]}\n"
            count += 1
        return fullAnnotation


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
        self.annotation = ""

    def setupNode(self, plan, child):

        # Set up basics

        self.nodeType = plan['Node Type']
        self.cost = plan['Total Cost']
        self._isChildNode = child

        self.annotatePlan(plan)

    def annotatePlan(self, plan):

        # SORT --------------------------------------------------------------------------------------
        if self.nodeType == "Sort":
            # Attr used to Sort
            attr = ",".join(plan['Sort Key'])
            self.annotation = f"<b>{self.nodeType}</b> operation was performed. Rows are sorted to be in order based on key(s): (<b>{attr}</b>)"

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
            self.annotation = f"<b>{self.nodeType}</b> operation was performed ({plan['Join Type']})."

        # BITMAP SCAN -----------------------------------------------------------------------------------------------
        elif self.nodeType == "Bitmap Heap Scan":
            table = plan['Relation Name']
            self.annotation = f"<b>{self.nodeType}</b> operation was performed on table:{table} "

         # BITMAP INDEX SCAN -----------------------------------------------------------------------------------------------
        elif self.nodeType == "Bitmap Index Scan":
            index = plan['Index Name']
            cond = plan['Index Cond']
            self.annotation = f"<b>{self.nodeType}</b> operation was performed with index:{index} and condition: {cond}"

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
