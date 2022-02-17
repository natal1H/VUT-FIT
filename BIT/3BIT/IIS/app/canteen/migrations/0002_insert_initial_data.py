# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.db import models, migrations

def load_canteen_from_sql():
    import os
    PROJECT_DIR = os.path.abspath(os.path.dirname(__name__))
    sql_statements = open(os.path.join(PROJECT_DIR,'canteen/sql/initial_data.sql'), 'r').read()
    return sql_statements


def delete_canteen_with_sql():
    return 'DELETE from canteen_facility; DELETE from canteen_menu; DELETE from canteen_item; DELETE from canteen_person; DELETE from canteen_food_order; DELETE from canteen_facility_menus; DELETE from canteen_menu_items; DELETE from canteen_food_order_item;'

class Migration(migrations.Migration):

    dependencies = [
        ('canteen', '0001_create_models'),
    ]

    operations = [
        migrations.RunSQL(load_canteen_from_sql(), delete_canteen_with_sql()),
    ]