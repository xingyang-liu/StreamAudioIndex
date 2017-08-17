import heapq
import random


class TopkHeap(object):
    def __init__(self, k):
        self.k = k
        self.data = []

    def Push(self, elem):
        # elem["value"] = -elem["value"]
        tup = (elem["value"], str(elem["note"])) # add a negative signal before elem["value"] if you want minimum
        if len(self.data) < self.k:
            heapq.heappush(self.data, tup)
        else:
            topk_small = self.data[0]
            if tup[0] > topk_small[0]:
                heapq.heapreplace(self.data, tup)

    def TopK(self):
        return [{"value": x[0], "note": x[1]} for x in reversed([heapq.heappop(self.data) for x in xrange(len(self.data))])]
        # add a negative signal before x[0] if you did it above
#
# if __name__ == "__main__":
#     print "Hello"
#     list_rand = random.sample(xrange(1000000), 100)
#     th = TopkHeap(3)
#     for i in list_rand:
#         th.Push(i)
#     print th.TopK()
#     print sorted(list_rand, reverse=True)[-3:]