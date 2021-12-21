import numpy as np
import pandas as pd

def df_softmax(df):
    #m = df.mean(axis=1, level=level)
    #df = df.subtract(m,axis=0)
    df = df.apply(np.exp)
    s = df.sum(axis=1)
    df = df.divide(s,axis=0)
    return df

def extract_policy(df, level=0):
    d = df[["left", "straight", "right"]]
    d = d.unstack("State")
    #Softmax
    m = d.mean(axis=1, level=level)
    d = d.subtract(m,axis=0)
    d = d.apply(np.exp)
    s = d.sum(axis=1, level=level)
    d = d.divide(s,axis=0)
    return d

def kl_div(i,j,policy_df):
    policy_i = policy_df.loc[i]
    policy_j = policy_df.loc[j]
    res = 0
    for k in ['left','straight','right']:
        res += policy_i[k] * np.log(policy_i[k]/policy_j[k])

    return res

