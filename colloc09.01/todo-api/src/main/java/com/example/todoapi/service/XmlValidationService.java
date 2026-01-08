package com.example.todoapi.service;

import com.example.todoapi.model.Task;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.xml.sax.SAXException;
import javax.xml.XMLConstants;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.Validator;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import java.util.List;

@Service
public class XmlValidationService {
    
    @Autowired
    private Schema taskSchema;
    
    private JAXBContext jaxbContext;
    
    public XmlValidationService() {
        try {
            jaxbContext = JAXBContext.newInstance(TasksWrapper.class, Task.class);
        } catch (JAXBException e) {
            throw new RuntimeException("Failed to create JAXBContext", e);
        }
    }
    
    // Wrapper класс для списка задач
    @javax.xml.bind.annotation.XmlRootElement(name = "tasks")
    @javax.xml.bind.annotation.XmlAccessorType(javax.xml.bind.annotation.XmlAccessType.FIELD)
    public static class TasksWrapper {
        @javax.xml.bind.annotation.XmlElement(name = "task")
        private List<Task> tasks;
        
        public TasksWrapper() {}
        
        public TasksWrapper(List<Task> tasks) {
            this.tasks = tasks;
        }
        
        public List<Task> getTasks() { return tasks; }
        public void setTasks(List<Task> tasks) { this.tasks = tasks; }
    }
    
    /**
     * Валидация XML строки по XSD схеме
     */
    public boolean validateXml(String xml) {
        if (xml == null || xml.trim().isEmpty()) {
            return false;
        }
        
        try {
            Validator validator = taskSchema.newValidator();
            validator.setProperty(XMLConstants.ACCESS_EXTERNAL_DTD, "");
            validator.setProperty(XMLConstants.ACCESS_EXTERNAL_SCHEMA, "");
            
            // Удаляем XML декларацию для валидации
            String xmlToValidate = xml;
            if (xmlToValidate.startsWith("<?xml")) {
                int end = xmlToValidate.indexOf("?>");
                if (end != -1) {
                    xmlToValidate = xmlToValidate.substring(end + 2).trim();
                }
            }
            
            StringReader reader = new StringReader(xmlToValidate);
            validator.validate(new StreamSource(reader));
            return true;
        } catch (SAXException | IOException e) {
            System.err.println("XML validation error: " + e.getMessage());
            return false;
        }
    }
    
    /**
     * Маршаллинг списка задач в XML
     */
    public String tasksToXml(List<Task> tasks) throws Exception {
        try {
            TasksWrapper wrapper = new TasksWrapper(tasks);
            Marshaller marshaller = jaxbContext.createMarshaller();
            marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, true);
            marshaller.setProperty(Marshaller.JAXB_ENCODING, "UTF-8");
            
            StringWriter writer = new StringWriter();
            marshaller.marshal(wrapper, writer);
            
            String xml = writer.toString();
            
            // Проверяем валидность
            boolean isValid = validateXml(xml);
            System.out.println("Generated XML validation result: " + isValid);
            
            return xml;
        } catch (JAXBException e) {
            throw new Exception("XML generation failed: " + e.getMessage(), e);
        }
    }
    
    /**
     * Маршаллинг одной задачи в XML
     */
    public String taskToXml(Task task) throws Exception {
        try {
            // Для одной задачи создаем отдельный контекст
            JAXBContext singleContext = JAXBContext.newInstance(Task.class);
            Marshaller marshaller = singleContext.createMarshaller();
            marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, true);
            marshaller.setProperty(Marshaller.JAXB_ENCODING, "UTF-8");
            
            StringWriter writer = new StringWriter();
            marshaller.marshal(task, writer);
            
            String xml = writer.toString();
            
            // Для валидации одной задачи оборачиваем в <tasks>
            String wrappedXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                               "<tasks>\n" + xml + "\n</tasks>";
            boolean isValid = validateXml(wrappedXml);
            System.out.println("Single task XML validation result: " + isValid);
            
            return xml;
        } catch (JAXBException e) {
            throw new Exception("XML generation failed: " + e.getMessage(), e);
        }
    }
}