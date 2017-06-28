
/*************************
 ***   ACTDET module   ***
 *************************/
SELECT public.VT_method_delete('activity_det', TRUE);

-- TODO: detection_threshold default value
SELECT public.VT_method_add(
    'activity_det',
    '{"(event,public.vtevent,out,true,,,)"}',
    '{"(keyframe_freq,int,true,25,[1\\,inf],)",
      "(min_width,double,true,0,[0\\,1],)",
      "(min_height,double,true,0,[0\\,1],)",
      "(max_width,double,true,1,[0\\,1],)",
      "(max_height,double,true,1,[0\\,1],)"}',
    FALSE,
    'Activity detection',
    NULL);
