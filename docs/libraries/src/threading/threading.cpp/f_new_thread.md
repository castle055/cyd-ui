---
layout: function
title: new_thread
owner: __MISSING__
brief: __MISSING__
tags:
  - function
defined_in_file: src/threading/threading.cpp
overloads:
  cydui::threading::thread_t * new_thread(void (*)(cydui::threading::thread_t *)):
    arguments:
      - description: __OPTIONAL__
        name: task
        type: void (cydui::threading::thread_t *)
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::threading::thread_t * new_thread(void (*)(cydui::threading::thread_t *) task)
  cydui::threading::thread_t * new_thread(void (*)(cydui::threading::thread_t *), void *):
    arguments:
      - description: __OPTIONAL__
        name: task
        type: void (cydui::threading::thread_t *)
      - description: __OPTIONAL__
        name: data
        type: void *
    description: __MISSING__
    return: __OPTIONAL__
    signature_with_names: cydui::threading::thread_t * new_thread(void (*)(cydui::threading::thread_t *) task, void * data)
---
