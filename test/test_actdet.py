# Tests activity detection module

import time
import sys
import vtclient.client as vtclient

# Connect to VTServer
cl = vtclient.VTServerClient("tcp://127.0.0.1:8719")


#################################################
# CONFIGURE THESE VALUES IF NEEDED
#################################################

module = 'activity_det'
params = [
    {'type':'TP_INT', 'name':'keyframe_freq' , 'value_int':25},
    {'type':'TP_FLOAT', 'name':'min_width' , 'value_float':0.1},
    {'type':'TP_FLOAT', 'name':'max_width' , 'value_float':0.9},
    {'type':'TP_FLOAT', 'name':'min_height' , 'value_float':0.1},
    {'type':'TP_FLOAT', 'name':'max_height' , 'value_float':0.9}]

test_dataset = 'test_actdet_py'
test_video = '/home/vojca/vid/demo_complete/video1.mpg'


#################################################
# BEGIN
#################################################

print 'TEMPORARY dataset : ' + test_dataset
print '--------------'

print 'add dataset:'
addDataset = cl.call('addDataset', {'name': test_dataset})
print addDataset
print '--------------'

print 'get dataset list:'
getDatasetList = cl.call('getDatasetList', {})
print getDatasetList
print '--------------'

print 'add video:'
addVideo = cl.call('addVideo', {
    'dataset_id': addDataset['dataset_id'],
    # 'name':'video32',                 # optional suggested name
    # 'location':'subdir/video32.mpg'   # optional suggested location in dataset
    'filepath': test_video,
    'start_time': {'seconds': 1439734399, 'nanos': 0}})
print addVideo
print '--------------'

print 'get video info:'
getVideoInfo = cl.call('getVideoInfo', {
    'dataset_id': addDataset['dataset_id'],
    'video_ids':[addVideo['video_id']]})
print getVideoInfo
print '--------------'

print 'add processing task:'
addTask = cl.call('addTask', {
    'dataset_id': addDataset['dataset_id'],
    'module':module,
    'params':params})
print addTask
print '--------------'

if addVideo['res']['success'] and addTask['res']['success']:
    print 'run process:'
    runProcess = cl.call('runProcess', {
        'dataset_id': addDataset['dataset_id'],
        'video_ids': [addVideo['video_id']],
        'task_id':addTask['task_id']})
    print runProcess
    print '--------------'

    print 'get progress:'
    p_finished = False
    while not p_finished:
        time.sleep(1)
        print 'process info:'
        getProcessInfo = cl.call('getProcessInfo', {
            'dataset_id': addDataset['dataset_id'],
            'process_ids': [runProcess['process_id']]})
        print getProcessInfo
        print '--------------'
        print 'task progress:'
        getTaskProgress = cl.call('getTaskProgress', {
            'dataset_id':addDataset['dataset_id'],
            'task_id':addTask['task_id'],
            'video_ids':[addVideo['video_id']]})
        print getTaskProgress
        print '--------------'
        if getTaskProgress['res']['success']:
            progress = getTaskProgress['task_progress']['progress']
            state = getProcessInfo['processes'][0]['state']
            p_finished = (progress >= 1.0) or (state == 4)

    print 'get events:'
    getEventList = cl.call('getEventList', {
        'dataset_id':addDataset['dataset_id'],
        'task_id':addTask['task_id'],
        'video_ids':[addVideo['video_id']]
        #,'filter':{'min_duration':1.8}
        #,'filter':{'begin_timewindow':{'seconds':1427861116,'nanos':0}}
        #,'filter':{'region':{'x1':0,'y1':0,'x2':0.0,'y2':0.0}}
        })
    print getEventList
    print '--------------'
    #sys.exit(0)

    print 'get event stats:'
    getEventList = cl.call('getEventsStats', {
        'dataset_id':addDataset['dataset_id'],
        'task_id':addTask['task_id'],
        'video_ids':[addVideo['video_id']]})
    print getEventList
    print '--------------'

# if addTask['res']['success']:
#     print 'delete task:'
#     print cl.call('deleteTask', {
#         'dataset_id': addDataset['dataset_id'],
#         'task_id':addTask['task_id']})
#     print '--------------'

# if addVideo['res']['success']:
#     print 'delete video:'
#     print cl.call('deleteVideo', {
#         'dataset_id': addDataset['dataset_id'],
#         'video_id': addVideo['video_id']})
#     print '--------------'

# print 'delete dataset:'
# print cl.call('deleteDataset', {'dataset_id': addDataset['dataset_id']})
