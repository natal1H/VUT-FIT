-- Inserting into 'facility' table
INSERT INTO canteen_facility (id_facility, address, name, deadline, max_ordered_meals, image)
    VALUES (1, 'Božetěchova 1, Brno-Královo Pole', 'Menza Starý pivovar', '23:55:00', 5, 'images/menza_stary_pivovar.jpg');
INSERT INTO canteen_facility (id_facility, address, name, deadline, max_ordered_meals, image)
    VALUES (2, 'Purkyňova 93, Brno-Královo Pole', 'Menza Purkyňova', '17:00:00', 5, 'images/menza_purkynova.jpg');
INSERT INTO canteen_facility (id_facility, address, name, deadline, max_ordered_meals, image)
    VALUES (3, 'Kolejní 2, Brno-Královo Pole', 'Menza Kolejní', '14:30:00', 5, 'images/menza_kolejni.jpg');

-- Inserting into 'menu' table
-- Menu Menza Stary Pivovar
INSERT INTO canteen_menu (id_menu, type, date, max_items)
    VALUES (1, 'd', '2019-11-25', 10);
INSERT INTO canteen_menu (id_menu, type, date, max_items)
    VALUES (2, 'd', '2020-01-31', 10);
INSERT INTO canteen_menu (id_menu, type, max_items)
    VALUES (3, 's', 10);
-- Menu Menza Purkynova
INSERT INTO canteen_menu (id_menu, type, date, max_items)
    VALUES (4, 'd', '2019-12-24', 10);
INSERT INTO canteen_menu (id_menu, type, max_items)
    VALUES (5, 's', 10);
-- Menu Menza Kolejni
INSERT INTO canteen_menu (id_menu, type, max_items)
    VALUES (6, 's', 10);

-- Inserting into 'item' table
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (1, 'n', 'Hotdog', 'Regular hotdog', 1.5, 'images/hotdog.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (2, 'n', 'Baguette', 'Baguette with salad, ham and tomatoes', 2.5, 'images/bageta.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (3, 'n', 'Pizza - cheese', 'Pizza with 4 kinds of cheese', 4.0, 'images/pizza_cheese.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (4, 'n', 'Pizza - Hawai', 'Pizza with pineapple on top and ham', 4.25, 'images/pizza_hawai.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (5, 'n', 'Pizza - Margheritta', 'Pizza Margherita', 4.00, 'images/pizza_margherita.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (6, 'n', 'Coca-cola', 'Coca-cola 0.3l', 0.75, 'images/cola.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (7, 'n', 'Chicken steak', 'Chicken steak with fries', 4.5 , 'images/chicken_steak.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (8, 'n', 'Water', 'Water bottle 0.5l', 0.6 , 'images/water.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (9, 'v', 'Salad', 'Green stuff', 3.5 , 'images/salad.jpg');
INSERT INTO canteen_item (id_item, diet_type, name, description, price, image)
    VALUES (10, 'g', 'Apple', 'Red apple', 0.4 , 'images/apple.jpg');

-- Inserting into 'person' table
INSERT INTO canteen_person (id_person, firstname, surname, address, telephone, email, role)
    VALUES (1, 'Jozef', 'Mrkvička', 'Purkyňova 93, 612 00 Brno', '0987654321', 'jozef.mrkvicka@mail.com', 'a');
INSERT INTO canteen_person (id_person, firstname, surname, address, telephone, email, role)
    VALUES (2, 'Ján', 'Kováč', 'Purkyňova 93, 612 00 Brno', '0987654322', 'jan.kovac@mail.com', 'o');
INSERT INTO canteen_person (id_person, firstname, surname, address, telephone, email, role)
    VALUES (3, 'Jana', 'Balážová', 'Purkyňova 93, 612 00 Brno', '0987654323', 'jana.balazova@mail.com', 'd');
INSERT INTO canteen_person (id_person, firstname, surname, address, telephone, email, role)
    VALUES (4, 'Anna', 'Molnárová', 'Purkyňova 93, 612 00 Brno', '0987654324', 'anna.molnarova@mail.com', 'r');

-- Inserting into 'food_order' table
INSERT INTO canteen_food_order (id_food_order, status, person_id, facility_id, date_created, date_approved, date_delivered, date_paid, approved_by_id, delivered_by_id)
   VALUES (1, 'a', 4, 1, '2019-10-16 15:26', '2019-10-16 17:26', null, '2019-10-16 15:26', 2, null);
INSERT INTO canteen_food_order (id_food_order, status, person_id, facility_id, date_created, date_approved, date_delivered, date_paid, approved_by_id, delivered_by_id)
   VALUES (2, 'd', 4, 3, '2019-12-02 11:22', '2019-12-02 13:00', '2019-12-02 14:00', '2019-12-02 11:22', 2, 3);

-- Inserting into 'facility_menus' table
INSERT INTO canteen_facility_menus (id_facility_id, id_menu_id)
    VALUES (1, 1);
INSERT INTO canteen_facility_menus (id_facility_id, id_menu_id)
    VALUES (1, 2);
INSERT INTO canteen_facility_menus (id_facility_id, id_menu_id)
    VALUES (1, 3);
INSERT INTO canteen_facility_menus (id_facility_id, id_menu_id)
    VALUES (2, 4);
INSERT INTO canteen_facility_menus (id_facility_id, id_menu_id)
    VALUES (2, 5);
INSERT INTO canteen_facility_menus (id_facility_id, id_menu_id)
    VALUES (3, 6);

-- Inserting into 'menu_items' table
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (1, 7);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (2, 7);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (3, 1);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (3, 2);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (3, 6);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (3, 8);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (4, 7);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (5, 3);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (5, 4);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (5, 5);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (5, 8);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (6, 8);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (6, 6);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (6, 9);
INSERT INTO canteen_menu_items (id_menu_id, id_item_id)
    VALUES (6, 10);


-- Inserting into 'food_order_item' table
INSERT INTO canteen_food_order_item (id_food_order_id, id_item_id, quantity)
    VALUES (1, 2, 1);
INSERT INTO canteen_food_order_item (id_food_order_id, id_item_id, quantity)
    VALUES (2, 9, 1);
INSERT INTO canteen_food_order_item (id_food_order_id, id_item_id, quantity)
    VALUES (2, 10, 1);
