from django.http import Http404

from django.shortcuts import render, get_object_or_404, redirect
from django.views import generic
from .models import *

from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.decorators import login_required

from django.contrib.auth.models import User

from django.http import HttpResponseRedirect

from .forms import *

from django.contrib.auth import login, authenticate

from django.views.generic import *

from datetime import date
from datetime import datetime

from django.contrib.auth.mixins import LoginRequiredMixin



def index(request):
    obj = Facility.objects.all()

    num_visits = request.session.get('num_visits', 0)
    request.session['num_visits'] = num_visits + 1

    context = {
        "object": obj,
        'num_visits': num_visits,
    }

    return render(request, 'index.html', context=context)


def search_view(request):
    search = request.GET.get('search_form')

    objects = Facility.objects.all()

    corr_search_objs = []

    for obj in objects:
        if search.lower() in obj.address.lower() or search.lower() in obj.name.lower():
            corr_search_objs.append(obj)

    context = {
        "result": search,
        "objects": corr_search_objs
    }

    return render(request, 'search_result.html', context=context)


def register_view(request):
    context = {

    }

    if request.method == "POST":
        form = UserCreationForm(request.POST)

        if form.is_valid():
            form.save()
            return redirect('loggend_on')


    else:
        form = UserCreationForm()

    return render(request, 'registration/register.html', {'form': form})


def pay_with_signup(request):
    if request.method == 'POST':

        form = SignUpForm(request.POST)
        session_id_food_order = request.session.get("cart_id", None)

        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')

            print(username)
            raw_password = form.cleaned_data.get('password1')

            firstname = form.cleaned_data.get('firstname')
            surname = form.cleaned_data.get('surname')
            address = form.cleaned_data.get('address')
            email = form.cleaned_data.get('email')
            telephone = form.cleaned_data.get('telephone')

            user = authenticate(username=username, password=raw_password)

            login(request, user)
            user_instance = User.objects.filter(username=username).first()
            print(user_instance)
            person_instance = Person.objects.create(user=user_instance, firstname=firstname, surname=surname,
                                                    address=address, telephone=telephone, role='r')

            food_order_instance = Food_order.objects.filter(id_food_order=session_id_food_order, status='o')

            food_order_instance.update(person=person_instance, status='p')

            return redirect('loggend_on')
    else:
        form = SignUpForm()

    return render(request, 'registration/register.html', {'form': form})

    pass


def signup(request):
    if request.method == 'POST':

        form = SignUpForm(request.POST)

        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')

            print(username)
            raw_password = form.cleaned_data.get('password1')

            firstname = form.cleaned_data.get('firstname')
            surname = form.cleaned_data.get('surname')
            address = form.cleaned_data.get('address')
            email = form.cleaned_data.get('email')
            telephone = form.cleaned_data.get('telephone')

            user = authenticate(username=username, password=raw_password)

            login(request, user)
            user_instance = User.objects.filter(username=username).first()
            print(user_instance)
            Person.objects.create(user=user_instance, firstname=firstname, surname=surname, address=address,
                                  telephone=telephone, role='r')

            return redirect('loggend_on')
    else:
        form = SignUpForm()

    return render(request, 'registration/register.html', {'form': form})


def logged_view(request):
    context = {

    }

    return render(request, 'registration/logged_on.html', context)


def dynamic_facility_view(request, id):
    try:
        menu = Facility_menus.objects.filter(id_facility=id)
    except Facility.DoesNotExist:
        raise Http404

    today = date.today()
    menu_objs = []
    for i in menu:

        menu_objs.append(i.id_menu)


    try:
        obj = Facility.objects.get(id_facility=id)
    except Facility.DoesNotExist:
        raise Http404

    context = {
        # "meals":facility_meals,
        "object": obj,
        "menu_objs": menu_objs
    }
    return render(request, 'facility_detail.html', context)


def filter_menu(request, id_menu, diet_type):
    try:
        menu = Menu_items.objects.filter(id_menu=id_menu)
    except Facility_menus.DoesNotExist:
        raise Http404
    item_objs = []

    for i in menu:
        if i.id_item.diet_type == diet_type:
            item_objs.append(i.id_item)

    facility = Facility_menus.objects.filter(id_menu=id_menu).first()

    facility_menus_instance = Facility_menus.objects.filter(id_menu=id_menu).first()
    print(facility_menus_instance.id_facility.id_facility)

    context = {
        "item_objs": item_objs,
        "menu": Menu.objects.get(id_menu=id_menu),
        "id_facility": facility_menus_instance.id_facility.id_facility,  
        "id_menu": id_menu,
        "facility": facility.id_facility  
    }

    return render(request, 'menu_detail.html', context)


def menu_view(request, id):
    try:
        menu = Menu_items.objects.filter(id_menu=id)
    except Facility_menus.DoesNotExist:
        raise Http404
    item_objs = []

    for i in menu:
        
        item_objs.append(i.id_item)

    facility = Facility_menus.objects.filter(id_menu=id).first()

    facility_menus_instance = Facility_menus.objects.filter(id_menu=id).first()

    context = {
        "item_objs": item_objs,
        "menu": Menu.objects.get(id_menu=id),
        "id_facility": facility_menus_instance.id_facility.id_facility,  
        "id_menu": id,
        "facility": facility.id_facility  
    }

    return render(request, 'menu_detail.html', context)


def add_to_cart(request, id_item, id_facility):
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    facility_instance_control = Facility.objects.filter(id_facility=id_facility).first()
    deadline = facility_instance_control.deadline
    deadline = deadline.strftime("%H:%M:%S")

    if current_time > deadline:
        return render(request, 'error_access.html', {
            "msg": "Unable to add item, its already after deadline"
        })

    # if user is logged in
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()

        food_order_instance = Food_order.objects.filter(person=person_instance, status='o').first()

        # if there is no food order

        if food_order_instance is None:
            print("Now we are makin new order")
            facility_instance = Facility.objects.filter(id_facility=id_facility).first()
            food_order_item_instance = Item.objects.filter(id_item=id_item).first()

            # Creates new food order

            food_order_instance = Food_order.objects.create(facility=facility_instance, person=person_instance,
                                                            status='o')  # Creates new food order
            Food_order_item.objects.create(id_food_order=food_order_instance,
                                           id_item=food_order_item_instance)  # adds the item to the order

        # If there is
        else:

            if food_order_instance.facility.id_facility != id_facility:
                print("you cant order from different facility")
                return render(request, 'error_access.html', {
                    "msg": "Unable to order from more facilities at once"
                })

            food_order_item_instance = Item.objects.filter(id_item=id_item).first()

            food_order_item_instance_from_mm_table = Food_order_item.objects.filter(id_food_order=food_order_instance,
                                                                                    id_item=food_order_item_instance)

            if food_order_item_instance_from_mm_table.count() == 1:
                quantity = food_order_item_instance_from_mm_table.first().quantity + 1
                print(quantity)
                Food_order_item.objects.filter(id_food_order=food_order_instance,
                                               id_item=food_order_item_instance).update(quantity=quantity)
            else:
                Food_order_item.objects.create(id_food_order=food_order_instance, id_item=food_order_item_instance)

    # ak nie je vytovrená

    # unregistered
    else:

        session_id_food_order = request.session.get("cart_id", None)
        food_order_instance = Food_order.objects.filter(id_food_order=session_id_food_order, status='o')

        # if there is an order
        if food_order_instance.exists():
            food_order_item_instance = Item.objects.filter(id_item=id_item).first()
            food_order_instance = Food_order.objects.filter(id_food_order=session_id_food_order, status='o').first()
            food_order_item_instance_from_mm_table = Food_order_item.objects.filter(id_food_order=food_order_instance,
                                                                                    id_item=food_order_item_instance).first()


            if food_order_instance.facility.id_facility != id_facility:
                print("you cant order from different facility")
                return render(request, 'error_access.html', {
                    "msg": "Unable to order from more facilities at once"
                })

            if food_order_item_instance_from_mm_table is None:

                Food_order_item.objects.create(id_food_order=food_order_instance, id_item=food_order_item_instance)

            else:

                quantity = food_order_item_instance_from_mm_table.quantity + 1
                print(quantity)
                Food_order_item.objects.filter(id_food_order=food_order_instance,
                                               id_item=food_order_item_instance).update(quantity=quantity)

        else:

            facility_instance = Facility.objects.filter(id_facility=id_facility).first()
            food_order_item_instance = Item.objects.filter(id_item=id_item).first()
            food_order_instance = Food_order.objects.create(facility=facility_instance, status='o')
            Food_order_item.objects.create(id_food_order=food_order_instance,
                                           id_item=food_order_item_instance)  # adds the item to the order

            request.session['cart_id'] = food_order_instance.id_food_order

    return HttpResponseRedirect(request.META.get('HTTP_REFERER'))  



def remove_from_cart(request, id_item, id_facility):
    if request.user.is_authenticated:

        item_instance = Item.objects.filter(id_item=id_item).first()
        person_instance = Person.objects.filter(user=request.user).first()
        food_order_instance = Food_order.objects.filter(person=person_instance, status='o').first()

        delete_instance = Food_order_item.objects.filter(id_item=item_instance,
                                                         id_food_order=food_order_instance).first()

        if delete_instance.quantity == 1:
            delete_instance.delete()
        else:
            quantity = delete_instance.quantity - 1
            print(quantity)
            Food_order_item.objects.filter(id_item=item_instance, id_food_order=food_order_instance).update(
                quantity=quantity)

        prob_food_order_delete = Food_order_item.objects.filter(id_food_order=food_order_instance)

        if prob_food_order_delete.exists():
            print("")
        else:
            
            food_order_instance.delete()



    else:
        cart_id = request.session.get("cart_id", None)
        food_order_instance = Food_order.objects.filter(id_food_order=cart_id, status='o').first()

        item_instance = Item.objects.filter(id_item=id_item).first()

        delete_instance = Food_order_item.objects.filter(id_item=item_instance,
                                                         id_food_order=food_order_instance).first()

        if delete_instance.quantity == 1:
            delete_instance.delete()
        else:
            quantity = delete_instance.quantity - 1
            print(quantity)
            Food_order_item.objects.filter(id_item=item_instance, id_food_order=food_order_instance).update(
                quantity=quantity)

        # checking if the food order still has something
        prob_food_order_delete = Food_order_item.objects.filter(id_food_order=food_order_instance)

        if prob_food_order_delete.exists():
            print("somethin here")
        else:
            print("gotta delete food_order")
            food_order_instance.delete()

    return HttpResponseRedirect(request.META.get('HTTP_REFERER'))  


# current order
def cart_view(request):
    
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        food_order_instance = Food_order.objects.filter(person=person_instance, status='o').first()
        

        food_order_items_list = Food_order_item.objects.filter(id_food_order=food_order_instance)
        price = 0

        print(food_order_items_list)

        for x in food_order_items_list:
            price = price + x.id_item.price * x.quantity


        if food_order_instance is None:
            facility_instance = None
        else:
            facility_instance = food_order_instance.facility

        unregistered = False

    else:
        unregistered = True
        cart_id = request.session.get("cart_id", None)
        food_order_instance = Food_order.objects.filter(id_food_order=cart_id, status='o').first()
        food_order_items_list = Food_order_item.objects.filter(id_food_order=food_order_instance)

        price = 0
        for x in food_order_items_list:
            price = price + x.id_item.price * x.quantity

        if food_order_instance is None:
            facility_instance = None
        else:
            facility_instance = food_order_instance.facility

    context = {
        "food_order_instance": food_order_instance,
        "food_order_items_list": food_order_items_list,
        "facility_instance": facility_instance,
        "unregistered": unregistered,
        "price": price
    }

    return render(request, 'cart.html', context)


def pay_view(request):
    if request.user.is_authenticated:

        person_instance = Person.objects.filter(user=request.user).first()
        Food_order.objects.filter(person=person_instance, status='o').update(status='p')

    else:
        cart_id = request.session.get("cart_id", None)

        my_form = Pay_form()
        if request.method == "POST":

            my_form = Pay_form(request.POST)

            if my_form.is_valid():
                print(my_form.cleaned_data["telephone"])

                person_instance = Person.objects.create(**my_form.cleaned_data)
                Food_order.objects.filter(id_food_order=cart_id, status='o').update(status='p', person=person_instance)
                context = {

                }
                return render(request, 'paid.html', context)

            else:
                print(my_form.errors)

        context = {
            "form": my_form
        }

        return render(request, 'pay_unregistered.html', context)

    return HttpResponseRedirect(request.META.get('HTTP_REFERER'))


# this is basically all orders view
# this is the order for one user and his/hers orders
def order_view(request):  # basically a cart

    if request.user.is_authenticated:

        person_instance = Person.objects.filter(user=request.user).first()
        food_order_qs = Food_order.objects.filter(person=person_instance)
        

        food_order_ordered = Food_order.objects.filter(person=person_instance, status='o')
        food_order_paid = Food_order.objects.filter(person=person_instance, status='p')

        food_order_approved = Food_order.objects.filter(person=person_instance, status='a')

        food_order_canceled = Food_order.objects.filter(person=person_instance, status='c')
        food_order_delivered = Food_order.objects.filter(person=person_instance, status='d')
        print(food_order_paid)

        context = {
            "food_order_ordered": food_order_ordered,
            "food_order_paid":food_order_paid,
            "food_order_approved": food_order_approved,
            "food_order_canceled": food_order_canceled,
            "food_order_delivered": food_order_delivered,
        }

    else:

        context = {
            "message": "it is not here"

        }

    return render(request, 'order.html', context)


@login_required
def detail_order_view(request, id):
    #print("hey")
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()

        

        food_order_instance =Food_order.objects.filter(id_food_order=id).first()
        if(person_instance.id_person!=food_order_instance.person.id_person):#test is tried accessed by other user
            raise Http404

        print(food_order_instance.person.id_person)
        
        food_order_items=Food_order_item.objects.filter(id_food_order=food_order_instance)
       
        price=0

        item_list=[]
        for x in food_order_items:
            
            price = price + x.id_item.price * x.quantity


            
        

    else:
        raise Http404

    context={
        "item_list": item_list,
        "food_order_items": food_order_items,
        "food_order":food_order_instance,
        "price":price,

    }
    
    return render(request, 'order_detail_view.html', context)

@login_required
def profile_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()

        context = {
            "person": person_instance,
            "role": person_instance.role
        }
    else:
        raise Http404

    return render(request, 'profile.html', context)


def admin_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()

        if person_instance.role != 'a':
            raise Http404
        context = {
            "role": person_instance.role
        }
    else:
        raise Http404

    return render(request, 'admin_view.html', context)


class Food_order_update_view(generic.UpdateView, LoginRequiredMixin):
    template_name = 'food_order_update_view.html'
    form_class = Food_order_form

    def get_object(self):
        id = self.kwargs.get("id")

        return get_object_or_404(Food_order, id_food_order=id)

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)



    def get_instance(self):
        id = self.kwargs.get("id")
        food_order_instance=Food_order.objects.filter(id_food_order=id).first()
        return food_order_instance

    """
    def get_context_data(self, **kwargs):
        context = super(UserProfileDetailView, self).get_context_data(**kwargs) # get the default context data
        context['voted_links'] = Link.objects.filter(votes__voter=self.request.user) # add extra field to the context
        return context
    """


@login_required
def food_order_assign_view(request, id_food_order):
    food_order_instance=Food_order.objects.filter(id_food_order=id_food_order).first()
    person_qs=Person.objects.filter(role='d')
    
    context={
        "person_qs":person_qs,
        "food_order_instance":food_order_instance
    }

    return render(request, 'food_order_assign.html', context)

@login_required
def food_order_assign_button_view(request, id_food_order, id_person):
    person_instance = Person.objects.filter(user=request.user).first()
    driver_instance=Person.objects.filter(id_person=id_person).first()
    food_order_instance=Food_order.objects.filter(id_food_order=id_food_order).update(delivered_by=driver_instance, approved_by=person_instance, status='a')
    

    context={}

    return render(request, 'success_assign.html', context)

class Food_order_delete_view(DeleteView, LoginRequiredMixin):
    template_name = 'food_order_delete_view.html'

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Food_order, id_food_order=id)

    def get_success_url(self):
        return '../food_order'



class person_update_view(generic.UpdateView, LoginRequiredMixin):
    template_name = 'person_update.html'
    form_class = person_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Person, id_person=id)

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)

    def get_success_url(self):
        return '../person_list_view'

class person_delete_view(DeleteView, LoginRequiredMixin):
    template_name = 'person_delete_view.html'

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Person, id_person=id)

    def get_success_url(self):
        return '../person_list_view'


def person_list_view(request):
    if request.user.is_authenticated:

        person_instance = Person.objects.filter(user=request.user).first()
        person_instance_list = Person.objects.all()
        
        if person_instance.role == 'a':

            context = {
                "person_instance_list": person_instance_list
            }
        else:
            raise Http404


    else:
        raise Http404

    return render(request, 'person_list_view.html', context)


def food_order_list_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        
        person_instance.is_operator()

        food_order_list = Food_order.objects.all()

        food_order_list_test=list(food_order_list)

        today_list=[]

        for x in food_order_list_test:
            
            if datetime.today().strftime('%Y-%m-%d') ==x.date_created.strftime('%Y-%m-%d'):
                today_list.append(x)
        print(today_list)
        context = {
            "today_list":today_list,
            "food_order_list": food_order_list
        }
    else:
        raise Http404

    return render(request, 'Food_order_list.html', context)


def driver_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_driver()

        driver_orders_list = Food_order.objects.filter(delivered_by=person_instance, status='a')

        print(driver_orders_list)

        if person_instance.role == 'a' or person_instance.role == 'd':
            context = {
                "driver_orders_list": driver_orders_list,
                "role": person_instance.role
            }
        else:
            raise Http404




    else:
        raise Http404

    return render(request, 'driver_view.html', context)


"""
@login_required
def detail_order_view(request, id):
    #print("hey")
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()

        

        food_order_instance =Food_order.objects.filter(id_food_order=id).first()
        if(person_instance.id_person!=food_order_instance.person.id_person):#test is tried accessed by other user
            raise Http404

        print(food_order_instance.person.id_person)
        
        food_order_items=Food_order_item.objects.filter(id_food_order=food_order_instance)
       


        item_list=[]
        for x in food_order_items:
            
            item_list.append(x.id_item)
        

    else:
        raise Http404

    context={
        "item_list": item_list,
        "food_order_items": food_order_items,
        "food_order":food_order_instance

    }
    
    return render(request, 'order_detail_view.html', context)
"""

@login_required
def driver_food_order_info(request, id):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_driver()
    else:
        PermissionDenied()
        return render(request, 'error_access.html', {})

    food_order_instance = Food_order.objects.filter(id_food_order=id).first()

    person_customer_instance = food_order_instance.person
    food_order_items=Food_order_item.objects.filter(id_food_order=food_order_instance)

    food_order_items_list = Food_order_item.objects.filter(id_food_order=food_order_instance)
    price = 0

    print(food_order_items_list)

    for x in food_order_items_list:
        price = price + x.id_item.price * x.quantity

    context = {
        'food_order_instance': food_order_instance,
        'person_customer_instance': person_customer_instance,
        "food_order_items":food_order_items,
        "price":price
    }
    return render(request, 'driver_food_order_info.html', context)


@login_required
def driver_deliver(request, id):

    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_driver()
    else:
        PermissionDenied()
        return render(request, 'error_access.html', {})
    Food_order.objects.filter(id_food_order=id).update(status='d')

    return redirect('driver_view')

@login_required
def operator_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_operator()
       
        context = {
                "role": person_instance.role
        }
        

    else:
        raise Http404

    return render(request, 'operator_view.html', context)

@login_required
def food_list_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_operator()

    else:
        raise Http404

    food_list = Item.objects.all()

    context = {
        "food_list": food_list

    }

    return render(request, 'food_list_view.html', context)


class food_update_view(generic.UpdateView, LoginRequiredMixin):
    template_name = 'food_update_view.html'
    form_class = Food_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Item, id_item=id)

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)


class food_delete_view(DeleteView, LoginRequiredMixin):
    template_name = 'food_delete_view.html'
    form_class = Food_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Item, id_item=id)

    def get_success_url(self):
        return '../food_list_view'


class food_create_view(CreateView, LoginRequiredMixin):
    template_name = 'food_create_view.html'
    form_class = Food_form
    queryset = Item.objects.all()

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)

    def get_success_url(self):
        return '../food_list_view'


class menu_create_view(CreateView, LoginRequiredMixin):
    template_name = 'menu_create_view.html'
    form_class = Menu_form
    queryset = Menu.objects.all()  

  

    def form_valid(self, form):
        print(form.cleaned_data)
        form.cleaned_data
        return super().form_valid(form)

    def get_success_url(self):
        return '../menu_list_view'


class menu_update_view(UpdateView, LoginRequiredMixin):
    template_name = 'menu_update_view.html'
    form_class = Menu_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Menu, id_menu=id)

    def form_valid(self, form):
        id = self.kwargs.get("id")
        
        id_menu_check=Menu.objects.filter(id_menu=id).first()

        facilitiy_instance=Facility_menus.objects.filter(id_menu=id_menu_check).first()
        
        facilitiy_instance=facilitiy_instance.id_facility

        facility_menus=Facility_menus.objects.filter(id_facility=facilitiy_instance)

        if form.cleaned_data['type'] == 's':
            cnt=0
            for x in facility_menus:
                x=x.id_menu

                if x.type=='s':
                    print("Static is already there")
                    PermissionDenied()
                    raise Http404

        print(form.cleaned_data)

        return super().form_valid(form)


class menu_delete_view(DeleteView, LoginRequiredMixin):
    template_name = 'menu_delete_view.html'
    form_class = Menu_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Menu, id_menu=id)

    def get_success_url(self):
        return '../menu_list_view'


class Facility_menus_create_view(CreateView, LoginRequiredMixin):
    template_name = 'facility_menus_create.html'
    form_class = Facility_menus_form
    queryset = Facility_menus.objects.all()

    def form_valid(self, form):
        id_menu_check = form.cleaned_data['id_menu']
        if_facility_check = form.cleaned_data['id_facility']

        all_facility_menus_from_facility = Facility_menus.objects.filter(id_facility=if_facility_check)
        print(all_facility_menus_from_facility)

        if id_menu_check.type == 's':
            for x in all_facility_menus_from_facility:
                if x.id_menu.type == 's':
                    print("something bad")
                    PermissionDenied()
                    raise Http404

        print(id_menu_check)
        print(if_facility_check)

        print(form.cleaned_data)
        return super().form_valid(form)

    def get_success_url(self):
        return '../menu_list_view'


class Menu_items_create_view(CreateView, LoginRequiredMixin):
    template_name = 'menu_items_create.html'
    form_class = Menu_items_form
    queryset = Menu_items.objects.all()

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)

    def get_success_url(self):
        return '../menu_list_view'


def menu_list_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_operator()

    menu_instance_list = Menu.objects.all()

    context = {
        "menu_instance_list": menu_instance_list,
    }

    return render(request, 'menu_list_view.html', context)


def facility_list_staff_view(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_operator()

    facility_instance_list = Facility.objects.all()

    context = {
        "facility_instance_list": facility_instance_list,
    }

    return render(request, 'facility_staff_view.html', context)

    pass


class facility_create_view(CreateView, LoginRequiredMixin):
    template_name = 'facility_create_view.html'
    form_class = Facility_form
    queryset = Facility.objects.all()

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)

    def get_success_url(self):
        return '../facility_list_staff_view'


class facility_update_view(UpdateView, LoginRequiredMixin):
    template_name = 'facility_update_view.html'
    form_class = Facility_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Facility, id_facility=id)

    def form_valid(self, form):
        print(form.cleaned_data)
        return super().form_valid(form)

    def get_success_url(self):
        id = self.kwargs.get("id")
        return '../facility_update_view/' + str(id)


class facility_delete_view(DeleteView, LoginRequiredMixin):
    template_name = 'facility_delete_view.html'
    form_class = Facility_form

    def get_object(self):
        id = self.kwargs.get("id")
        return get_object_or_404(Facility, id_facility=id)

    def get_success_url(self):
        return '../facility_list_staff_view'


def facility_menus_list_staff(request, id):
    facility_menus_instance_list = Facility_menus.objects.filter(id_facility=id)

    print(facility_menus_instance_list)

    menu_list = []

    for x in facility_menus_instance_list:
        menu_list.append(x.id_menu)


    context = {
        "menu_list": menu_list
    }
    return render(request, 'facility_menus_list_staff.html', context)


def facility_menus_items_list_staff(request, id):
    facility_menus_items_list = Menu_items.objects.filter(id_menu=id)


    item_list = []

    for x in facility_menus_items_list:
        item_list.append(x.id_item)


    context = {
        "item_list": item_list,
        "id_menu": id
    }
    return render(request, 'facility_menus_items_list_staff.html', context)

@login_required
def delete_from_menu(request, id_menu, id_item):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_operator()
    else:
        PermissionDenied()
        return render(request, 'error_access.html', {})

    delete_instance = Menu_items.objects.filter(id_menu=id_menu, id_item=id_item).first()
    delete_instance.delete()
    return HttpResponseRedirect(request.META.get('HTTP_REFERER'))  


def controlPermissions(request, bool_var, person_instance):
    if bool_var:
        person_instance = Person.objects.filter(user=request.user).first()
        person_instance.is_admin()
        person_instance.is_operator()
    else:
        PermissionDenied()
        return render(request, 'error_access.html', {})


@login_required
def profile_edit(request):
    person_instance = Person.objects.filter(user=request.user).first()
    if request.method == 'POST':
        person_instance = Person.objects.filter(user=request.user).first()
        user_instance = request.user

        form = EditProfileForm(request.POST or None)

        if form.is_valid():

            firstname = form.cleaned_data.get('firstname')
            surname = form.cleaned_data.get('surname')
            address = form.cleaned_data.get('address')
            email = form.cleaned_data.get('email')
            telephone = form.cleaned_data.get('telephone')

            Person.objects.filter(id_person=person_instance.id_person).update(firstname=firstname,
                                                                              surname=surname,
                                                                              address=address,
                                                                              telephone=telephone,
                                                                              email=email)

            person_instance = Person.objects.filter(user=request.user).first()


    else:
        form = EditProfileForm()

    context = {
        'form': form,
        'user_profile': request.user,
        'person': person_instance,

    }

    return render(request, 'profile_edit.html', context)



