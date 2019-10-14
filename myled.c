/*
 * myled.c
 *
 * Copyright (C) 2019 Ryuichi Ueda <ryuichiueda@gmail.com>
 * Copyright (C) 2019 Satoshi Kimura <s17c1043hf@s.chibakoudai.jp>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <asm/delay.h>
#include <linux/delay.h>
MODULE_AUTHOR("Kimura Satoshi");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
  char c;
  int i;
  if(copy_from_user(&c,buf,sizeof(char)))
    return -EFAULT;

  if(c == '0'){
        gpio_base[10] = 1 << 25;
	}
	else if(c == '1'){
	   for(i=0; i<1*2; i++){
		   gpio_base[7] = 1 << 25;
		   msleep(300);
		   gpio_base[10] = 1 << 25;
		   msleep(300);
	   }
	}
	else if(c == '2'){
		for(i=0; i<2*2; i++){
			gpio_base[7] = 1 << 25;
			msleep(300);
			gpio_base[10] = 1 << 25;
			msleep(300);
		}
	}
	else if(c == '3'){
		for(i=0; i<3*2; i++){
			gpio_base[7] = 1 << 25;
			msleep(300);
			gpio_base[10] = 1 << 25;
			msleep(300);
		}
	}

    return 1;
}

static struct file_operations led_fops = {
        .owner = THIS_MODULE,
        .write = led_write
};


static int __init init_mod(void) //カーネルモジュールの初期化
{
	int retval;
  gpio_base = ioremap_nocache(0x3f200000, 0xA0);
  const u32 led = 25;
  const u32 index = led/10;
  const u32 shift = (led%10)*3;
  const u32 mask = ~(0x7 << shift);
  gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);
  retval =  alloc_chrdev_region(&dev, 0, 1, "myled");
    	if(retval < 0){
        	printk(KERN_ERR "alloc_chrdev_region failed.\n");
        	return retval;
    	}
	printk(KERN_INFO "retval = %d, %s is loaded. major:%d\n",retval,__FILE__,MAJOR(dev));
        cdev_init(&cdv, &led_fops);
        retval = cdev_add(&cdv, dev, 1);
        if(retval < 0){
                printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
                return retval;
        }
	cls = class_create(THIS_MODULE,"myled");   //ここから追加
        if(IS_ERR(cls)){
                printk(KERN_ERR "class_create failed.");
                return PTR_ERR(cls);
        }
	device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));
	return 0;
}

static void __exit cleanup_mod(void) //後始末
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);     // マクロで関数を登録
module_exit(cleanup_mod);  // 同上
