from django.urls import path
from . import views

from django.contrib.auth.views import LoginView, LogoutView,PasswordChangeView,PasswordChangeDoneView



#basic
urlpatterns = [
    path('', views.index, name='index'),
    path('search', views.search_view, name='search'),
    path('facility/<int:id>', views.dynamic_facility_view, name='facility-view'),
    path('facility_list_staff_view', views.facility_list_staff_view, name='facility_list_staff_view'),
    path('facility_create_view', views.facility_create_view.as_view(), name='facility_create_view'),
    path('facility_update_view/<int:id>', views.facility_update_view.as_view(), name='facility_update_view'),
    path('facility_delete_view/<int:id>', views.facility_delete_view.as_view(), name='facility_delete_view'),
    path('facility_menus_list_staff/<int:id>', views.facility_menus_list_staff, name='facility_menus_list_staff'),
    path('facility_menus_items_list_staff/<int:id>', views.facility_menus_items_list_staff, name='facility_menus_items_list_staff'),
    path('facility_menus_create', views.Facility_menus_create_view.as_view(), name='facility_menus_create'),
    path('menu_items_create', views.Menu_items_create_view.as_view(), name='menu_items_create'),
    path('filter_menu/<int:id_menu>/<str:diet_type>', views.filter_menu, name='filter_menu'),
    path('delete_from_menu/<int:id_menu>/<int:id_item>', views.delete_from_menu, name='delete_from_menu'),
    path('menu/<int:id>', views.menu_view, name='menu'),

    path('order',views.order_view, name='order'),
    path('detail_order_view/<int:id>/',views.detail_order_view, name='detail_order_view'),
    
    path('cart',views.cart_view, name='cart'),
    path('pay',views.pay_view, name='pay'),
    path('add_to_cart/<int:id_item>/<int:id_facility>', views.add_to_cart, name='add_to_cart'),
    path('remove_from_cart/<int:id_item>/<int:id_facility>', views.remove_from_cart, name='remove_from_cart'),

]

# for admin processes
urlpatterns += [
    path('admin_view', views.admin_view, name='admin_view'),
    path('driver_view', views.driver_view, name='driver_view'),
    path('food_order/', views.food_order_list_view, name='food_order'),
    path('food_order_update/<int:id>', views.Food_order_update_view.as_view(), name='food_order_update'),
    path('food_order_delete/<int:id>', views.Food_order_delete_view.as_view(), name='food_order_delete'),
    path('deliver_order/<int:id>', views.driver_food_order_info, name='deliver_order'),
    path('deliver_order_finally/<int:id>', views.driver_deliver, name='deliver_order_finally'),
    
    path('person_update/<int:id>', views.person_update_view.as_view(), name='person_update'),
    path('person_list_view', views.person_list_view, name='person_list_view'),
    path('person_delete/<int:id>', views.person_delete_view.as_view(), name='person_delete'),


    path('menu_list_view', views.menu_list_view, name='menu_list_view'),
    path('menu_create_view', views.menu_create_view.as_view(), name='menu_create_view'),
    path('menu_update_view/<int:id>', views.menu_update_view.as_view(), name='menu_update_view'),
    path('menu_delete_view/<int:id>', views.menu_delete_view.as_view(), name='menu_delete_view'),
]

urlpatterns += [
    path('operator_view', views.operator_view, name='operator_view'),
    path('food_create_view', views.food_create_view.as_view(), name='food_create'),
    path('food_list_view', views.food_list_view, name='food_list_view'),
    path('food_update_view/<int:id>', views.food_update_view.as_view(), name='food_update_view'),
    path('food_delete_view/<int:id>', views.food_delete_view.as_view(), name='food_delete_view'),

    path('food_order_assign/<int:id_food_order>', views.food_order_assign_view, name='food_order_assign'),
    path('food_order_assign_button_view/<int:id_food_order>/<int:id_person>', views.food_order_assign_button_view, name='food_order_assign_button_view'),
]

# user authentication
urlpatterns += [
    path('register', views.signup, name='register'),
    path('register_with_pay', views.pay_with_signup, name='register_with_pay'),
    path('login', LoginView.as_view(), name='login_url'),
    path('logged', views.logged_view, name='loggend_on'),
    path('logout', LogoutView.as_view(next_page='/'), name='logout'),
    path('profile', views.profile_view, name='profile'),
    path('profile_edit', views.profile_edit, name='profile_edit'),
    path('change_password', PasswordChangeView.as_view( template_name='password_change_form.html'), name='change_password'),
    path('password_change_done', PasswordChangeDoneView.as_view( template_name='password_change_done.html'), name='password_change_done'),
]