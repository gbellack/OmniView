from nltk import sent_tokenize, word_tokenize, pos_tag, ne_chunk
from pprint import pprint
import nltk

def get_human_names(text):
    tokens = nltk.tokenize.word_tokenize(text)
    pos = nltk.pos_tag(tokens)
    sentt = nltk.ne_chunk(pos, binary = False)
    person_list = []
    person = []
    name = ""
    for subtree in sentt.subtrees(filter=lambda t: t.label() == 'PERSON'):
        for leaf in subtree.leaves():
            person.append(leaf[0])
        if len(person) > 1: #avoid grabbing lone surnames
            for part in person:
                name += part + ' '
            if name[:-1] not in person_list:
                person_list.append(name[:-1])
        else:
            name = leaf[0]
            if name not in person_list:
                person_list.append(name)
        
        name = ''
        person = []

    return (person_list)