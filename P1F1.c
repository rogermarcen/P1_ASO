/**
 * @file   P1F1.c
 * @author Roger Marcen
 * @date   2021/2022
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>                 // Required for the GPIO functions
#include <linux/interrupt.h>            // Required for the IRQ code

static unsigned int gpioLEDvermell = 23;    //GPIO23 pel LED Vermell
static unsigned int gpioLEDverd = 24;   //GPIO24 pel LED Verd
static unsigned int gpioButtonOnVerd = 27;    //GPIO27 per encendre LED Verd
static unsigned int gpioButtonOffVerd = 22;     //GPIO22 per apagar LED Verd
static unsigned int gpioButtonOnVermell = 5;    //GPIO5 per encendre LED Vermell
static unsigned int gpioButtonOffVermell = 6;   //GPIO6 per apagar LED Vermell
static unsigned int irqNumberOnVerd;
static unsigned int irqNumberOffVerd;
static unsigned int irqNumberOnVermell;
static unsigned int irqNumberOffVermell;      
static bool	ledOn = 0;          

static irq_handler_t  encendreLEDverd(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  apagarLEDverd(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  encendreLEDvermell(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  apagarLEDvermell(unsigned int irq, void *dev_id, struct pt_regs *regs);


static irq_handler_t encendreLEDverd(unsigned int irq, void *dev_id, struct pt_regs *regs){
   ledOn = true;                          
   gpio_set_value(gpioLEDverd, ledOn);          
   return (irq_handler_t) IRQ_HANDLED;      
}

static irq_handler_t encendreLEDvermell(unsigned int irq, void *dev_id, struct pt_regs *regs){
   ledOn = true;                          
   gpio_set_value(gpioLEDvermell, ledOn);          
   return (irq_handler_t) IRQ_HANDLED;      
}

static irq_handler_t apagarLEDverd(unsigned int irq, void *dev_id, struct pt_regs *regs){
   ledOn = false;                          
   gpio_set_value(gpioLEDverd, ledOn);          
   return (irq_handler_t) IRQ_HANDLED;      
}

static irq_handler_t apagarLEDvermell(unsigned int irq, void *dev_id, struct pt_regs *regs){
   ledOn = false;                          
   gpio_set_value(gpioLEDvermell, ledOn);          
   return (irq_handler_t) IRQ_HANDLED;      
}

static int __init my_init(void){
    int result = 0;
    printk(KERN_INFO "Inicialitzant LEDs LKM\n");
    // Is the GPIO a valid GPIO number (e.g., the BBB has 4x32 but not all available)
    if (!gpio_is_valid(gpioLEDvermell)){
        printk(KERN_INFO "Invalid LED GPIO\n");
    }
    if (!gpio_is_valid(gpioLEDverd)){
        printk(KERN_INFO "Invalid LED GPIO\n");
    }
    // Iniciem LED Verd
    ledOn = true;
    gpio_request(gpioLEDverd, "sysfs");          
    gpio_direction_output(gpioLEDverd, ledOn);   
    gpio_export(gpioLEDverd, false);

    // Iniciem LED Vermell
    gpio_request(gpioLEDvermell, "sysfs");          
    gpio_direction_output(gpioLEDvermell, ledOn);   
    gpio_export(gpioLEDvermell, false);  

    // Iniciem boto On Verd
    gpio_request(gpioButtonOnVerd, "sysfs");       
    gpio_direction_input(gpioButtonOnVerd);       
    gpio_set_debounce(gpioButtonOnVerd, 200);      
    gpio_export(gpioButtonOnVerd, false);

    // Iniciem boto Off Verd
    gpio_request(gpioButtonOffVerd, "sysfs");       
    gpio_direction_input(gpioButtonOffVerd);       
    gpio_set_debounce(gpioButtonOffVerd, 200);      
    gpio_export(gpioButtonOffVerd, false);

    // Iniciem boto On Vermell
    gpio_request(gpioButtonOnVermell, "sysfs");       
    gpio_direction_input(gpioButtonOnVermell);       
    gpio_set_debounce(gpioButtonOnVermell, 200);      
    gpio_export(gpioButtonOnVermell, false); 

    // Iniciem boto Off Vermell
    gpio_request(gpioButtonOffVermell, "sysfs");       
    gpio_direction_input(gpioButtonOffVermell);       
    gpio_set_debounce(gpioButtonOffVermell, 200);      
    gpio_export(gpioButtonOffVermell, false);
                                
    // Relacionem les interrupcions amb els botons
    irqNumberOnVerd = gpio_to_irq(gpioButtonOnVerd);
    irqNumberOffVerd = gpio_to_irq(gpioButtonOffVerd);
    irqNumberOnVermell = gpio_to_irq(gpioButtonOnVermell);
    irqNumberOffVermell = gpio_to_irq(gpioButtonOffVermell);

    // This next call requests an interrupt line
    result = request_irq(irqNumberOnVerd, (irq_handler_t) encendreLEDverd, IRQF_TRIGGER_RISING, "Enecendre_LED_Verd", NULL);                 
    result = request_irq(irqNumberOffVerd, (irq_handler_t) apagarLEDverd, IRQF_TRIGGER_RISING, "Apagar_LED_Verd", NULL);
    result = request_irq(irqNumberOnVermell, (irq_handler_t) encendreLEDvermell, IRQF_TRIGGER_RISING, "Enecendre_LED_Vermell", NULL);                 
    result = request_irq(irqNumberOffVermell, (irq_handler_t) apagarLEDvermell, IRQF_TRIGGER_RISING, "Apagar_LED_Vermell", NULL);                 

    return result;
}

static void __exit my_exit(void){
   gpio_set_value(gpioLEDverd, 0);              
   gpio_unexport(gpioLEDverd);
   gpio_set_value(gpioLEDvermell, 0);              
   gpio_unexport(gpioLEDvermell);                  
   free_irq(irqNumberOffVerd, NULL);
   free_irq(irqNumberOnVerd, NULL);
   free_irq(irqNumberOffVermell, NULL);
   free_irq(irqNumberOnVermell, NULL);               
   gpio_unexport(gpioButtonOnVerd);
   gpio_unexport(gpioButtonOffVerd);
   gpio_unexport(gpioButtonOnVermell);
   gpio_unexport(gpioButtonOffVermell);
   gpio_free(gpioLEDverd);
   gpio_free(gpioLEDvermell);                      
   gpio_free(gpioButtonOnVerd);
   gpio_free(gpioButtonOffVerd);
   gpio_free(gpioButtonOnVermell);
   gpio_free(gpioButtonOffVermell); 
   return;                  
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
