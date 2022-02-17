from .models import *

def add_person_role_to_context(request):
    if request.user.is_authenticated:
        person_instance = Person.objects.filter(user=request.user).first()

        return {
            "show_menus": True,
            "person": person_instance,
            "role": person_instance.role
        }
    else:
        return {
            "show_menus": False
        }