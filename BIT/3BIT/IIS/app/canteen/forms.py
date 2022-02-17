from django import forms
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User
from .models import *

from django.views.generic.edit import *


class NameForm(forms.Form):
    your_name = forms.CharField(label='Your name', max_length=100)

class SearchForm(forms.Form):


    class Meta:
        model =Person

    pass

class Food_order_form(forms.ModelForm):
    class Meta:
        model = Food_order
        fields =[
            'id_food_order',
            #'date_created',
            #'date_paid',
            #'date_approved',
            #'date_delivered',
            #'person',
            #'facility',
            #'delivered_by',
            'status'
        ]

class person_form(forms.ModelForm):
    class Meta:
        model = Person
        fields =[
            'id_person',
            'firstname',
            'surname',
            'address',
            'telephone',
            'user',
            'role',
        ]

class Food_form(forms.ModelForm):
    class Meta:
        model = Item
        fields =[
            'id_item',
            'name',
            'description',
            'price',
            'image',
            'diet_type'
        ]

class Pay_form(forms.ModelForm):
    class Meta:
        model = Person
        fields =[
            
            'firstname',
            'surname',
            'address',
            'telephone',
            
        ]

class Menu_form(forms.ModelForm):
    class Meta:
        model = Menu
        fields =[
            
            'date',
            'max_items',
            'type',
            
        ]


class Facility_form(forms.ModelForm):
    class Meta:
        model = Facility
        fields =[
            
            'address',
            'name',
            'deadline',
            'max_ordered_meals',
            
        ]

class Facility_menus_form(forms.ModelForm):
    class Meta:
        model = Facility_menus
        fields =[
            
            'id_facility',
            'id_menu'
            
        ]

class Menu_items_form(forms.ModelForm):
    class Meta:
        model = Menu_items
        fields =[
            
            'id_menu',
            'id_item'
            
        ]

class UserUpdate(UpdateView):


    """
    model = Author
    fields = ['name']
    template_name_suffix = '_update_form'
    """


    pass

class SignUpForm(UserCreationForm):

    firstname = forms.CharField(max_length=30, required=False)
    surname = forms.CharField(max_length=30, required=False)
    address = forms.CharField(max_length=30, required=False)
    email = forms.EmailField(max_length=254)
    telephone = forms.CharField(max_length=30, required=False)

    class Meta:
        model = User
        fields = ['username', 'firstname', 'surname', 'email','address','telephone', 'password2', 'password1']

class EditProfileForm(forms.Form):
    #user = forms.CharField(max_length=30, required=False, help_text='First Name.')

    firstname = forms.CharField(max_length=30, required=False, widget=forms.TextInput(attrs={"class":"tmpclass"}))
    surname = forms.CharField(max_length=30, required=False)
    address = forms.CharField(max_length=30, required=False)
    email = forms.EmailField(max_length=254)
    telephone = forms.CharField(max_length=30, required=False)

